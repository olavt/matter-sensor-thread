/*
 * MatterLightSensor.h
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#pragma once

#include "AmbientLightSensor.h"

#include <memory>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

using namespace chip;

class MatterLightSensor
{

public:

  MatterLightSensor(chip::EndpointId lightSensorEndpointId, std::shared_ptr<AmbientLightSensor> ambientLightSensor);

  void UpdateMeasurements();

private:

  EndpointId m_lightSensorEndpointId;
  std::shared_ptr<AmbientLightSensor> m_ambientLightSensor;

  void UpdateLightSensorAttributes(EndpointId lightSensorEndpointId, float ambientLightLux);

};
