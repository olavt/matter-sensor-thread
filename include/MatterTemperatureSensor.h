#pragma once

#include "TemperatureSensor.h"

#include <memory>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

using namespace chip;

// A Temperature Sensor device reports measurements of temperature.

class MatterTemperatureSensor
{

public:

  MatterTemperatureSensor(chip::EndpointId temperatureEndpointId, std::shared_ptr<TemperatureSensor> temperatureSensor);

  void UpdateMeasurements();

private:

  EndpointId m_temperatureEndpointId;
  std::shared_ptr<TemperatureSensor> m_temperatureSensor;

  void UpdateTemperatureAttributes(EndpointId endpoint, float temperatureCelsius);

};
