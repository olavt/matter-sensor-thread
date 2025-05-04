/*
 * MatterPressureSensor.cpp
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#include "MatterPressureSensor.h"

#include <cmath>
#include <app-common/zap-generated/attributes/Accessors.h>
#include "silabs_utils.h"

MatterPressureSensor::MatterPressureSensor(EndpointId pressureSensorEndpointId, std::shared_ptr<BarometricPressureSensor> barometricPressureSensor)
: m_pressureSensorEndpointId(pressureSensorEndpointId), m_barometricPressureSensor(barometricPressureSensor)
{
}

void MatterPressureSensor::UpdateMeasurements()
{
  auto barometricPressure = m_barometricPressureSensor->MeasureBarometricPressure();
  if (barometricPressure)
    {
      SILABS_LOG("[INFO] Updating ambient light measurement.");

      UpdatePressureSensorAttributes(m_pressureSensorEndpointId, barometricPressure.value());
    }
}

void MatterPressureSensor::UpdatePressureSensorAttributes(EndpointId m_pressureSensorEndpointId, float barometricPressureKiloPascal)
{
  int16_t measuredValue = (barometricPressureKiloPascal * 10 + 0.5);
  SILABS_LOG("[INFO] UpdatePressureMeasuredValue: measuredValue=%d", measuredValue);

  int8_t scale;
  chip::app::Clusters::PressureMeasurement::Attributes::Scale::Get(m_pressureSensorEndpointId, &scale);
  float scaleFactor = std::pow(10.0, scale);
  int16_t scaledValue = (barometricPressureKiloPascal * scaleFactor + 0.5);
  SILABS_LOG("[INFO] UpdatePressureMeasuredValue: scaledValue=%d", scaledValue);
  chip::app::Clusters::PressureMeasurement::Attributes::MeasuredValue::Set(m_pressureSensorEndpointId, measuredValue);
  chip::app::Clusters::PressureMeasurement::Attributes::ScaledValue::Set(m_pressureSensorEndpointId, scaledValue);
}


