/*
 * CO2Sensor.h
 *
 *  Created on: Mar 14, 2024
 *      Author: olavt
 */

#pragma once

class Co2Sensor
{

public:

  virtual ~Co2Sensor(){}

  virtual bool Initialize() = 0;

  bool IsInitialized();

  virtual bool MeasureCo2Level(float* co2Level) = 0;

protected:

  bool _isInitialized = false;

};
