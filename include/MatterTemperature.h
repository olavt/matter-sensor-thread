/*
 * MatterTemperature.h
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#pragma once

#include "TemperatureSensor.h"

#include <memory>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

using namespace chip;

class MatterTemperature
{

public:

  MatterTemperature(chip::EndpointId temperatureEndpointId, std::shared_ptr<TemperatureSensor> temperatureSensor);

  void UpdateMeasurements();

private:

  EndpointId m_temperatureEndpointId;
  std::shared_ptr<TemperatureSensor> m_temperatureSensor;

  void UpdateTemperatureAttributes(EndpointId endpoint, float temperatureCelsius);

};
