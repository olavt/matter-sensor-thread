/*
 * MatterLightSensor.cpp
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#include "MatterLightSensor.h"

#include <cmath>
#include <app-common/zap-generated/attributes/Accessors.h>
#include "silabs_utils.h"

MatterLightSensor::MatterLightSensor(EndpointId lightSensorEndpointId, std::shared_ptr<AmbientLightSensor> ambientLightSensor)
: m_lightSensorEndpointId(lightSensorEndpointId), m_ambientLightSensor(ambientLightSensor)
{
}

void MatterLightSensor::UpdateMeasurements()
{
  auto ambientLightLux = m_ambientLightSensor->MeasureAmbientLight();
  if (ambientLightLux)
    {
      SILABS_LOG("[INFO] Updating ambient light measurement.");

      UpdateLightSensorAttributes(m_lightSensorEndpointId, ambientLightLux.value());
    }
}

void MatterLightSensor::UpdateLightSensorAttributes(EndpointId lightSensorEndpointId, float ambientLightLux)
{
  // This attribute SHALL indicate the illuminance in Lux (symbol lx) as follows:
  // MeasuredValue = 10,000 x log10(illuminance) + 1,
  double value = 10000 * log10(ambientLightLux) + 1;
  uint16_t measuredValue = static_cast<uint16_t>(value);

  chip::app::Clusters::IlluminanceMeasurement::Attributes::MeasuredValue::Set(lightSensorEndpointId, measuredValue);
}


