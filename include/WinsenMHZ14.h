/*
 * WinsenMHZ14.h
 *
 *  Created on: Jan 5, 2024
 *      Author: olavt
 */

#pragma once

#include "Co2Sensor.h"
#include "sl_iostream.h"

class WinsenMHZ14 : public Co2Sensor
{

public:

  WinsenMHZ14(sl_iostream_t* stream);

  bool DisableAutomaticBaselineCorrection();

  bool EnableAutomaticBaselineCorrection();

  bool Initialize();

  bool MeasureCo2Level(float* co2Level);

private:

  bool _automaticBaselineCorrection = false;

  uint8_t _buffer[100];
  sl_iostream_t* _stream;

  uint8_t CalculateChecksum(uint8_t *buffer, int length);

  bool ChecksumOk(uint8_t *buffer, int length);

  bool SetAutomaticBaselineCorrection(bool automaticBaselineCorrection);

};

