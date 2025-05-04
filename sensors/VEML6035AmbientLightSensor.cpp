/*
 * VEML6035AmbientLightSensor.cpp
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#include "VEML6035AmbientLightSensor.h"

#include "sl_i2cspm_instances.h"
#include "sl_veml6035.h"
#include "silabs_utils.h"

bool VEML6035AmbientLightSensor::Init()
{
  if (_isInitialized)
    return true;

  sl_status_t status = sl_veml6035_init(sl_i2cspm_sensor, false);

  _isInitialized = (status == SL_STATUS_OK);
  if (status != SL_STATUS_OK)
    SILABS_LOG("[ERROR] VEML6035AmbientLightSensor.Init: Initialization failed with status=%d", status);

  return _isInitialized;
}

std::vector<Sensor::Measurement> VEML6035AmbientLightSensor::ReadAllMeasurements()
{
    std::vector<Measurement> measurements;
    if (!_isInitialized) {
        return measurements;
    }

    if (auto ambientLight = MeasureAmbientLight()) {
        measurements.push_back({MeasurementType::AmbientLight, *ambientLight});
    }

    return measurements;
}

std::optional<float> VEML6035AmbientLightSensor::MeasureAmbientLight()
{
    if (!_isInitialized) {
        return std::nullopt;
    }


    float ambientLightLux;
    sl_status_t status = sl_veml6035_get_als_lux(sl_i2cspm_sensor, &ambientLightLux);

    bool success = (status == SL_STATUS_OK);
    if (!success) {
        return std::nullopt;
    }

    return ambientLightLux;
}


