#include "SensirionSCD30.h"
#include <stdint.h>
#include "sensirion/scd30_i2c.h"
#include "sensirion/sensirion_common.h"
#include "sensirion/sensirion_i2c_hal.h"

void SensirionSCD30::Init()
{
  sensirion_i2c_hal_init();
  scd30_init(SCD30_I2C_ADDR_61);

  AirQualitySensor::Init();
}

std::set<AirQualitySensor::MeasurementType> SensirionSCD30::GetSupportedMeasurements() const
{
    // Return the measurement types supported by SEN66
    return {
        AirQualitySensor::MeasurementType::AmbientHumidity,
        AirQualitySensor::MeasurementType::AmbientTemperature,
        AirQualitySensor::MeasurementType::CO2
    };
}

std::vector<AirQualitySensor::Measurement> SensirionSCD30::ReadAllMeasurements()
{
  float co2_concentration;
  float temperature;
  float humidity;

  std::vector<AirQualitySensor::Measurement> measurements;

  int16_t local_error = 0;
  local_error = scd30_await_data_ready();
  if (local_error != NO_ERROR) {
      return measurements; // Return an empty vector on error
  }

  local_error =
      scd30_read_measurement_data(&co2_concentration, &temperature, &humidity);
  if (local_error != NO_ERROR) {
      return measurements; // Return an empty vector on error
  }


  measurements.push_back({MeasurementType::AmbientHumidity, humidity});
  measurements.push_back({MeasurementType::AmbientTemperature, temperature});
  measurements.push_back({MeasurementType::CO2, static_cast<float>(co2_concentration)});

  return measurements;
}

int SensirionSCD30::SetSensorAltitude(float altitude)
{
  int16_t status = scd30_set_altitude_compensation(altitude);
  return status;
}

int SensirionSCD30::StartContinuousMeasurement()
{
  int16_t status = scd30_start_periodic_measurement(0);

  return status;
}
