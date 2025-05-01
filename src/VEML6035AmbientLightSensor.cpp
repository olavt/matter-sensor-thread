/*
 * VEML6035AmbientLightSensor.cpp
 *
 *  Created on: Jul 3, 2024
 *      Author: olavt
 */

#include "VEML6035AmbientLightSensor.h"
#include "sl_i2cspm_instances.h"
#include "sl_veml6035.h"
#include "silabs_utils.h"

VEML6035AmbientLightSensor::VEML6035AmbientLightSensor()
{
}

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

bool VEML6035AmbientLightSensor::MeasureIllumination(float* lux)
{
  if(!_isInitialized) {
      return false;
  }

  sl_status_t sc = sl_veml6035_get_als_lux(sl_i2cspm_sensor, lux);

  bool success = (sc == SL_STATUS_OK);

  return success;
}
