/*
 * MatterTemperature.cpp
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#include "MatterTemperature.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include "silabs_utils.h"

MatterTemperature::MatterTemperature(EndpointId temperatureEndpointId, std::shared_ptr<TemperatureSensor> temperatureSensor)
: m_temperatureEndpointId(temperatureEndpointId), m_temperatureSensor(temperatureSensor)
{
}

void MatterTemperature::UpdateMeasurements()
{
  auto temperature = m_temperatureSensor->MeasureTemperature();
  if (temperature)
    {
      SILABS_LOG("[INFO] Updating temperature measurement.");

      UpdateTemperatureAttributes(m_temperatureEndpointId, temperature.value());
    }
}

void MatterTemperature::UpdateTemperatureAttributes(EndpointId endpoint, float temperatureCelsius)
{
  int16_t reportedTemperature = (temperatureCelsius * 100 + 0.5);
  SILABS_LOG("[INFO] UpdateTemperatureMeasuredValue: reportedTemperature=%d", reportedTemperature);
  chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Set(m_temperatureEndpointId, reportedTemperature);
}
