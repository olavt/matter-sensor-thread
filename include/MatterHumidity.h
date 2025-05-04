/*
 * MatterHumidity.h
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#pragma once

#include "RelativeHumiditySensor.h"

#include <memory>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

using namespace chip;

class MatterHumidity
{

public:

  MatterHumidity(chip::EndpointId humidityEndpointId, std::shared_ptr<RelativeHumiditySensor> humiditySensor);

  void UpdateMeasurements();

private:

  EndpointId m_humidityEndpointId;
  std::shared_ptr<RelativeHumiditySensor> m_humiditySensor;

  void UpdateHumidityAttributes(EndpointId endpoint, float relativeHumidity);

};
