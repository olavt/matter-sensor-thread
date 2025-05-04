/*
 * BMP3xxPressureSensor.cpp
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */


#include "BMP3xxPressureSensor.h"

#include "sl_i2cspm_instances.h"
#include "sl_bmp3xx.h"
#include "silabs_utils.h"

bool BMP3xxPressureSensor::Init()
{
  if (_isInitialized)
    return true;

  int8_t status = sl_bmp3xx_init(sl_i2cspm_sensor);

  _isInitialized = (status == SL_STATUS_OK);
  if (status != SL_STATUS_OK)
    SILABS_LOG("[ERROR] BMP3xxPressureSensor.Init: Initialization failed with status=%d", status);

  return _isInitialized;
}

std::vector<Sensor::Measurement> BMP3xxPressureSensor::ReadAllMeasurements()
{
    std::vector<Measurement> measurements;
    if (!_isInitialized) {
        return measurements;
    }

    if (auto barometricPressure = MeasureBarometricPressure()) {
        measurements.push_back({MeasurementType::BarometricPressure, *barometricPressure});
    }

    return measurements;
}

std::optional<float> BMP3xxPressureSensor::MeasureBarometricPressure()
{
    if (!_isInitialized) {
        return std::nullopt;
    }


    float pressure;

    // Returns pressure in Pascal
    int8_t result = sl_bmp3xx_measure_pressure(sl_i2cspm_sensor, &pressure);

    bool success = (result == SL_STATUS_OK);
    if (!success) {
        return std::nullopt;
    }

    float barometricPressure = (pressure / 1000.0f);

    return barometricPressure;
}




