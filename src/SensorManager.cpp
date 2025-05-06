/*
 * SensorManager.cpp
 *
 *  Created on: Dec 17, 2024
 *      Author: olavt
 */

#include <MatterAirQualitySensor.h>
#include <MatterHumiditySensor.h>
#include <MatterLightSensor.h>
#include <MatterPressureSensor.h>
#include <MatterTemperatureSensor.h>
#include <platform/CHIPDeviceLayer.h>
#include "sl_matter_sensor_config.h"
#include "silabs_utils.h"

#include <SensorManager.h>
#include "AirQualitySensor.h"
#include "CO2Sensor.h"
#include "SensirionSCD30.h"

using namespace chip;
using namespace chip::app;

#define TEMPERATURE_SENSOR_ENDPOINT 1
#define HUMIDITY_SENSOR_ENDPOINT 2
#define LIGHT_SENSOR_ENDPOINT 3
#define PRESSURE_SENSOR_ENDPOINT 4
#define AIR_QUALITY_SENSOR_ENDPOINT 5

constexpr chip::System::Clock::Seconds32 kSensorReadPeriod = chip::System::Clock::Seconds32(SL_MATTER_SENSOR_TIMER_PERIOD_S);

std::shared_ptr<BMP3xxPressureSensor> pressureSensor;
std::shared_ptr<Si70xxTemperatureHumiditySensor> temperatureHumiditySensor;
std::shared_ptr<VEML6035AmbientLightSensor> ambientLightSensor;

std::unique_ptr<MatterTemperatureSensor> matterTemperature;
std::unique_ptr<MatterHumiditySensor> matterHumidity;
std::unique_ptr<MatterLightSensor> matterLightSensor;
std::unique_ptr<MatterPressureSensor> matterPressureSensor;
std::unique_ptr<MatterAirQualitySensor> matterAirQuality;


namespace SensorManager
{

CHIP_ERROR Init()
{
    // Wait a bit for allowing some time to setup a debug connection
    vTaskDelay(pdMS_TO_TICKS(5000));

    CHIP_ERROR status = CHIP_NO_ERROR;

    vTaskDelay(pdMS_TO_TICKS(1000));

    //TemperatureSensor temperatureSensor = temperatureHumiditySensor;

    pressureSensor = std::make_shared<BMP3xxPressureSensor>();
    pressureSensor->Init();

    temperatureHumiditySensor = std::make_shared<Si70xxTemperatureHumiditySensor>();
    temperatureHumiditySensor->Init();

    ambientLightSensor = std::make_shared<VEML6035AmbientLightSensor>();
    ambientLightSensor->Init();

    auto airQualitySensor = std::make_shared<SensirionSCD30>(610.0);
    airQualitySensor->Init();

    //microphone.Init();

    //char instanceName[] = "exp";
    //sl_iostream_t* stream = sl_iostream_get_handle(instanceName);

    //co2Sensor = new WinsenMHZ14(stream);
    //co2Sensor->Initialize();
    //if (co2Sensor->IsInitialized())
    //  return status;

    //delete co2Sensor;

    // Create the classes to handle each endpoint.
    // The ZAP-file will contain matching definitions.

    // Endpoint 1: Temperature Sensor Device
    matterTemperature = std::make_unique<MatterTemperatureSensor>(TEMPERATURE_SENSOR_ENDPOINT, temperatureHumiditySensor);

    // Endpoint 2: Humidity Sensor Device
    matterHumidity = std::make_unique<MatterHumiditySensor>(HUMIDITY_SENSOR_ENDPOINT, temperatureHumiditySensor);

    // Endpoint 3: Light Sensor Device
    matterLightSensor = std::make_unique<MatterLightSensor>(LIGHT_SENSOR_ENDPOINT, ambientLightSensor);

    // Endpoint 4: Pressure Sensor Device
    matterPressureSensor = std::make_unique<MatterPressureSensor>(PRESSURE_SENSOR_ENDPOINT, pressureSensor);

    // Endpoint 6: Air Quality Device
    matterAirQuality = std::make_unique<MatterAirQualitySensor>(AIR_QUALITY_SENSOR_ENDPOINT, airQualitySensor);

    // Schedule the first execution of SensorTimerTriggered.
    // ScheduleWork is done to make sure it executes from the Matter task
    VerifyOrDieWithMsg(DeviceLayer::PlatformMgr().ScheduleWork([](intptr_t arg) {
      SensorTimerTriggered(&chip::DeviceLayer::SystemLayer(), nullptr);
    }) == CHIP_NO_ERROR,
                       AppServer, "Failed to schedule the first SensorCallback!");

    return status;
}

void MeasureSoundLevel()
{
  //float measuredSoundLevel;
  //microphone.MeasureSoundLevel(&measuredSoundLevel);
}

void UpdateMeasurements()
{
  SILABS_LOG("[INFO] Updating measurements.");

  auto ambientPressure = pressureSensor->MeasureBarometricPressure();
  if (ambientPressure)
    {
      matterAirQuality->SetAmbientPressure(ambientPressure.value());
    }

  matterTemperature->UpdateMeasurements();
  matterHumidity->UpdateMeasurements();
  matterLightSensor->UpdateMeasurements();
  matterPressureSensor->UpdateMeasurements();
  matterAirQuality->UpdateMeasurements();

  MeasureSoundLevel();
}

// This function executes from the Matter task. The use of LockChipStack / UnlockChipStack
// is not required when updating matter attributes
void SensorTimerTriggered(chip::System::Layer * aLayer, void * aAppState)
{
  UpdateMeasurements();

  // The chip::System::Layer::StartTimer will execute the function from the Matter task
  CHIP_ERROR err = aLayer->StartTimer(kSensorReadPeriod, SensorTimerTriggered, nullptr);
  if (err != CHIP_NO_ERROR) {
      SILABS_LOG("[ERROR] Failed to start sensor timer: %s", chip::ErrorStr(err));
  }
}

void ButtonActionTriggered(AppEvent * aEvent)
{
  UpdateMeasurements();
}

}





