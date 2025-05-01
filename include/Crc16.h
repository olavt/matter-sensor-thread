/*
 * Crc16.h
 *
 *  Created on: Feb 2, 2024
 *      Author: olavt
 */

#pragma once

#include <cstdint>
#include <vector>

class Crc16
{

  public:

static uint16_t CalculateChecksum(const uint8_t buffer[], int start, int length);

};
