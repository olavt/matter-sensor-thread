/*
 * MatterPressureSensor.h
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#pragma once

#include "BarometricPressureSensor.h"

#include <memory>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

using namespace chip;

class MatterPressureSensor
{

public:

  MatterPressureSensor(chip::EndpointId pressureSensorEndpointId, std::shared_ptr<BarometricPressureSensor> barometricPressureSensor);

  void UpdateMeasurements();

private:

  EndpointId m_pressureSensorEndpointId;
  std::shared_ptr<BarometricPressureSensor> m_barometricPressureSensor;

  void UpdatePressureSensorAttributes(EndpointId pressureSensorEndpointId, float barometricPressure);

};
