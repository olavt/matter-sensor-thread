/*
 * SensorManager.cpp
 *
 *  Created on: Dec 17, 2024
 *      Author: olavt
 */

#include <cmath>
#include <platform/CHIPDeviceLayer.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <platform/silabs/platformAbstraction/SilabsPlatform.h>

#include "sl_matter_sensor_config.h"
#include "silabs_utils.h"

#include <SensorManager.h>
#include "AirQualitySensor.h"
#include "MatterAirQuality.h"
#include "CO2Sensor.h"
#include "SensirionSCD30.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer::Silabs;
using namespace chip::Protocols::InteractionModel;

#define TEMPERATURE_SENSOR_ENDPOINT 1
#define HUMIDITY_SENSOR_ENDPOINT 2
#define ILLUMINANCE_SENSOR_ENDPOINT 3
#define PRESSURE_SENSOR_ENDPOINT 4
#define AIR_QUALITY_SENSOR_ENDPOINT 5

constexpr chip::System::Clock::Seconds32 kSensorReadPeriod = chip::System::Clock::Seconds32(SL_MATTER_SENSOR_TIMER_PERIOD_S);

BMP3xxPressureSensor pressureSensor;
Si70xxTemperatureHumiditySensor temperatureHumiditySensor;
VEML6035AmbientLightSensor illuminanceSensor;
//Co2Sensor* co2Sensor;
AirQualitySensor* airQualitySensor;
MatterAirQuality* matterAirQuality;

namespace SensorManager
{

void SensorTimerTriggered(chip::System::Layer * aLayer, void * aAppState)
{
  UpdateMeasurements();

  aLayer->StartTimer(kSensorReadPeriod, SensorTimerTriggered, nullptr);
}

CHIP_ERROR Init()
{
    // Wait a bit for allowing some time to setup a debug connection
    vTaskDelay(pdMS_TO_TICKS(5000));

    CHIP_ERROR status = CHIP_NO_ERROR;

    vTaskDelay(pdMS_TO_TICKS(1000));

    illuminanceSensor.Init();
    pressureSensor.Init();
    temperatureHumiditySensor.Init();
    //microphone.Init();

    //char instanceName[] = "exp";
    //sl_iostream_t* stream = sl_iostream_get_handle(instanceName);

    //co2Sensor = new WinsenMHZ14(stream);
    //co2Sensor->Initialize();
    //if (co2Sensor->IsInitialized())
    //  return status;

    //delete co2Sensor;

    std::unique_ptr<AirQualitySensor> airQualitySensor = std::make_unique<SensirionSCD30>();
    airQualitySensor->Init();

    matterAirQuality = new MatterAirQuality(std::move(airQualitySensor), AIR_QUALITY_SENSOR_ENDPOINT);
    matterAirQuality->StartMeasurements();

    SensorTimerTriggered(&chip::DeviceLayer::SystemLayer(), nullptr);

    return status;
}

void UpdatePressureMeasuredValue(float measuredPressureKiloPascal)
{
  int16_t measuredValue = (measuredPressureKiloPascal * 10 + 0.5);
  SILABS_LOG("[INFO] UpdatePressureMeasuredValue: measuredValue=%d", measuredValue);

  //chip::DeviceLayer::PlatformMgr().LockChipStack();

  int8_t scale;
  chip::app::Clusters::PressureMeasurement::Attributes::Scale::Get(PRESSURE_SENSOR_ENDPOINT, &scale);
  float scaleFactor = std::pow(10.0, scale);
  int16_t scaledValue = (measuredPressureKiloPascal * scaleFactor + 0.5);
  SILABS_LOG("[INFO] UpdatePressureMeasuredValue: scaledValue=%d", scaledValue);
  chip::app::Clusters::PressureMeasurement::Attributes::MeasuredValue::Set(PRESSURE_SENSOR_ENDPOINT, measuredValue);
  chip::app::Clusters::PressureMeasurement::Attributes::ScaledValue::Set(PRESSURE_SENSOR_ENDPOINT, scaledValue);

  //chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}

void UpdateTemperatureMeasuredValue(EndpointId endpoint, float temperatureCelsius)
{
  int16_t reportedTemperature = (temperatureCelsius * 100 + 0.5);
  SILABS_LOG("[INFO] UpdateTemperatureMeasuredValue: reportedTemperature=%d", reportedTemperature);
  //chip::DeviceLayer::PlatformMgr().LockChipStack();
  chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Set(endpoint, reportedTemperature);
  //chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}

void UpdateHumidityMeasuredValue(EndpointId endpoint, float relativeHumidity)
{
  SILABS_LOG("[INFO] UpdateHumidityMeasuredValue: relativeHumidity=%f", relativeHumidity);
  //chip::DeviceLayer::PlatformMgr().LockChipStack();
  chip::app::Clusters::RelativeHumidityMeasurement::Attributes::MeasuredValue::Set(endpoint, relativeHumidity * 100);
  //chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}

void UpdateRelativeHumidityMeasurement()
{
  float measuredRelativeHumidity;
  if (temperatureHumiditySensor.MeasureRelativeHumidity(&measuredRelativeHumidity))
  {
      SILABS_LOG("[INFO] Updating humidity measurement.");

      UpdateHumidityMeasuredValue(HUMIDITY_SENSOR_ENDPOINT, measuredRelativeHumidity);
  }
}

void UpdateIlluminanceMeasurement()
{
  float measuredLux;
  if (illuminanceSensor.MeasureIllumination(&measuredLux))
  {
      SILABS_LOG("[INFO] Updating illuminance measurement.");

      // This attribute SHALL indicate the illuminance in Lux (symbol lx) as follows:
      // MeasuredValue = 10,000 x log10(illuminance) + 1,
      double value = 10000 * log10(measuredLux) + 1;
      uint16_t measuredValue = static_cast<uint16_t>(value);

      //chip::DeviceLayer::PlatformMgr().LockChipStack();
      chip::app::Clusters::IlluminanceMeasurement::Attributes::MeasuredValue::Set(ILLUMINANCE_SENSOR_ENDPOINT, measuredValue);
      //chip::DeviceLayer::PlatformMgr().UnlockChipStack();
  }
}

void UpdateTemperatureMeasurement()
{
  float temperatureCelsius;
  if (temperatureHumiditySensor.MeasureTemperature(&temperatureCelsius))
    UpdateTemperatureMeasuredValue(TEMPERATURE_SENSOR_ENDPOINT, temperatureCelsius);
}

void UpdatePressureMeasurement()
{
  float measuredPressure;
  if (pressureSensor.MeasurePressure(&measuredPressure))
    UpdatePressureMeasuredValue(measuredPressure);
}

void MeasureSoundLevel()
{
  //float measuredSoundLevel;
  //microphone.MeasureSoundLevel(&measuredSoundLevel);
}

void UpdateAirQualityMeasurements()
{
  matterAirQuality->MeasureAirQuality();
}

void UpdateMeasurements()
{
  SILABS_LOG("[INFO] Updating measurements.");
  UpdateAirQualityMeasurements();
  UpdateIlluminanceMeasurement();
  UpdateRelativeHumidityMeasurement();
  UpdateTemperatureMeasurement();
  UpdatePressureMeasurement();
  MeasureSoundLevel();
}

void ButtonActionTriggered(AppEvent * aEvent)
{
  UpdateMeasurements();
}

}





