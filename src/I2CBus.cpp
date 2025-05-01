/*
 * I2CBus.cpp
 *
 *  Created on: Mar 8, 2024
 *      Author: olavt
 */

#include "I2CBus.h"

I2CBus::I2CBus(uint16_t address, sl_i2cspm_t* i2cspm)
{
  _address = address;
  _i2cspm = i2cspm;
}

bool I2CBus::WriteCommand(uint16_t command, std::span<uint8_t> arguments)
{
  int write_data_length = 2 + arguments.size();

  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_write_data[write_data_length];

  seq.addr  = _address << 1;
  seq.flags = I2C_FLAG_WRITE;

  i2c_write_data[0] = (command & 0xFF00) >> 8;
  i2c_write_data[1] = (command & 0x00FF);
  for (int i=0; i < arguments.size(); i++)
    i2c_write_data[2 + i] = arguments[i];

  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = write_data_length;

  seq.buf[1].data = 0;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(_i2cspm, &seq);
  if (ret != i2cTransferDone)
    return false;

  return true;
}

bool I2CBus::ReadData(std::span<uint8_t> readBuffer)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr  = _address << 1;
  seq.flags = I2C_FLAG_READ;
  seq.buf[0].data   = readBuffer.data();
  seq.buf[0].len    = readBuffer.size();

  seq.buf[1].data = 0;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(_i2cspm, &seq);
  if (ret != i2cTransferDone)
    return false;

  return true;

}


