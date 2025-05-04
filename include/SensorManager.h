/*
 * SensorManagerCustom.h
 *
 *  Created on: Dec 17, 2024
 *      Author: olavt
 */

#pragma once

#include <lib/core/CHIPError.h>
#include "AppEvent.h"

#include "BMP3xxPressureSensor.h"
#include "Si70xxTemperatureHumiditySensor.h"
#include "VEML6035AmbientLightSensor.h"

namespace SensorManager
{

  CHIP_ERROR Init();

  void UpdateMeasurements();

  void UpdatePressureMeasuredValue(float measuredPressureKiloPascal);

  void UpdateTemperatureMeasuredValue(float temperatureCelsius);

  void UpdateCO2Measurement();

  void UpdateRelativeHumidityMeasurement();

  void UpdateIlluminanceMeasurement();

  void UpdateTemperatureMeasurement();

  void UpdatePressureMeasurement();

  void MeasureSoundLevel();

  void SensorTimerTriggered(chip::System::Layer * aLayer, void * aAppState);

  void ButtonActionTriggered(AppEvent * aEvent);

};
