/*
 * Si70xxTemperatureHumiditySensor.cpp
 *
 *  Created on: May 3, 2025
 *      Author: olavt
 */

#include "Si70xxTemperatureHumiditySensor.h"

#include <cstddef>
#include "Si70xxTemperatureHumiditySensor.h"
#include "sl_i2cspm_instances.h"
#include "sl_si70xx.h"
#include "silabs_utils.h"

bool Si70xxTemperatureHumiditySensor::Init()
{
  if (_isInitialized)
    return true;

  sl_status_t status;

  status = sl_si70xx_init(sl_i2cspm_sensor, SI7021_ADDR);

  _isInitialized = (status == SL_STATUS_OK);
  if (status != SL_STATUS_OK)
    SILABS_LOG("[ERROR] Si70xxTemperatureHumiditySensor.Init: Initialization failed with status=%d", status);

  return _isInitialized;
}

std::vector<Sensor::Measurement> Si70xxTemperatureHumiditySensor::ReadAllMeasurements()
{
    std::vector<Measurement> measurements;
    if (!_isInitialized) {
        return measurements;
    }

    if (auto humidity = MeasureRelativeHumidity()) {
        measurements.push_back({MeasurementType::RelativeHumidity, *humidity});
    }
    if (auto temperature = MeasureTemperature()) {
        measurements.push_back({MeasurementType::Temperature, *temperature});
    }
    return measurements;
}

std::optional<float> Si70xxTemperatureHumiditySensor::MeasureRelativeHumidity()
{
    if (!_isInitialized) {
        return std::nullopt;
    }

    sl_status_t sc;
    uint32_t rhData;
    int32_t tData;

    sc = sl_si70xx_measure_rh_and_temp(sl_i2cspm_sensor, SI7021_ADDR, &rhData, &tData);

    bool success = (sc == SL_STATUS_OK);
    if (!success) {
        return std::nullopt;
    }

    float humidity = (rhData / 1000.0);

    return humidity;
}

std::optional<float> Si70xxTemperatureHumiditySensor::MeasureTemperature()
{
    if (!_isInitialized) {
        return std::nullopt;
    }

    sl_status_t sc;
    uint32_t rhData;
    int32_t tData;

    sc = sl_si70xx_measure_rh_and_temp(sl_i2cspm_sensor, SI7021_ADDR, &rhData, &tData);

    bool success = (sc == SL_STATUS_OK);
    if (!success) {
        return std::nullopt;
    }

    float temperature = (tData / 1000.0);

    return temperature;
}

