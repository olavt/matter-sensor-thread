/*
 * BMP3xxPressureSensor.cpp
 *
 *  Created on: Jul 3, 2024
 *      Author: olavt
 */

#include "BMP3xxPressureSensor.h"
#include "sl_i2cspm_instances.h"
#include "sl_bmp3xx.h"
#include "silabs_utils.h"

BMP3xxPressureSensor::BMP3xxPressureSensor()
{
}

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

bool BMP3xxPressureSensor::MeasurePressure(float* pressureResult)
{
  if (!_isInitialized)
    return false;

  float pressure;

  // Returns pressure in Pascal
  int8_t result = sl_bmp3xx_measure_pressure(sl_i2cspm_sensor, &pressure);

  bool success = (result == SL_STATUS_OK);
  if (success)
  {
      // Convert to kiloPascal (kPa)
      *pressureResult = (pressure / 1000.0);
  }

  return success;
}
