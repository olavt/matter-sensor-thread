#pragma once

#include "AmbientLightSensor.h"

#include <memory>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

using namespace chip;

// A Light Sensor device is a measurement and sensing device that is capable of measuring and
// reporting the intensity of light (illuminance) to which the sensor is being subjected.

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
