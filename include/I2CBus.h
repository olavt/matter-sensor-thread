/*
 * I2CBus.h
 *
 *  Created on: Mar 7, 2024
 *      Author: olavt
 */

#pragma once

#include <span>
#include "sl_status.h"
#include "sl_i2cspm.h"

class I2CBus
{

public:

  I2CBus(uint16_t address, sl_i2cspm_t* i2cspm);

  bool WriteCommand(uint16_t command, std::span<uint8_t> arguments = std::span<uint8_t>());

  bool ReadData(std::span<uint8_t> readBuffer);

private:

  sl_i2cspm_t* _i2cspm;
  uint16_t _address;

};
