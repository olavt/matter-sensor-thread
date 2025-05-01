/*
 * WinsenMHZ14.cpp
 *
 *  Created on: Jan 5, 2024
 *      Author: olavt
 */

#include <WinsenMHZ14.h>
#include "FreeRTOS.h"
#include "task.h"
#include "sl_iostream_uart.h"
#include "silabs_utils.h"

static const uint8_t ENABLE_ABC[] = { 0xFF, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xE6 };
static const uint8_t DISABLE_ABC[] = { 0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86 };
static const uint8_t READ_GAS_CONCENTRATION[] = { 0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79 };

WinsenMHZ14::WinsenMHZ14(sl_iostream_t* stream)
{
  _stream = stream;
}

bool WinsenMHZ14::Initialize()
{
  if (_isInitialized)
  {
    SILABS_LOG("[WARNING] WinsenMHZ14::Initialize: Is already initialized.");
    return false;
  }

  if (_stream == NULL)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::MeasureCo2Level: _stream is NULL.");
    return false;
  }

  // Use non-blocking reads
  sl_iostream_uart_set_read_block((sl_iostream_uart_t*)_stream, false);

  // Flush the input buffer
  size_t bytesRead;
  sl_iostream_read(_stream, _buffer, sizeof(_buffer), &bytesRead);

  int retryCount = 0;
  while (retryCount < 3)
  {
    if (EnableAutomaticBaselineCorrection())
    {
      _isInitialized = true;
      break;
    }


    retryCount++;
  }


  if (_isInitialized)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14.Init: Initialization succeeded.");
  }
  else
  {
    SILABS_LOG("[ERROR] WinsenMHZ14.Init: Initialization failed.");
  }

  return _isInitialized;
}

bool WinsenMHZ14::MeasureCo2Level(float* co2Level)
{
  SILABS_LOG("[INFO] WinsenMHZ14::MeasureCo2Level: Entering.");

  if (_stream == NULL)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::MeasureCo2Level: _stream is NULL.");
    return false;
  }

  sl_status_t status = sl_iostream_write(_stream, READ_GAS_CONCENTRATION, 9);
  if (status != SL_STATUS_OK)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::MeasureCo2Level: sl_iostream_write returned status=%d.", status);
    return false;
  }

  // Wait a bit to allow sensor to respond before reading response
  vTaskDelay(pdMS_TO_TICKS(400));

  size_t bytesRead;
  status = sl_iostream_read(_stream, _buffer, sizeof(_buffer), &bytesRead);
  if (status != SL_STATUS_OK)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::MeasureCo2Level: sl_iostream_read returned status=%d.", status);
    return false;
  }

  // Check expected response length
  if (bytesRead != 9)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::MeasureCo2Level: Invalid response length, bytesRead=%d.", bytesRead);
    return false;
  }

  // Check the checksum
  if (!ChecksumOk(_buffer, bytesRead))
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::MeasureCo2Level: Invalid checksum.");
    return false;
  }

  *co2Level = (int32_t)((_buffer[2] << 8) | _buffer[3]);

  return true;
}

bool WinsenMHZ14::DisableAutomaticBaselineCorrection()
{
  return SetAutomaticBaselineCorrection(false);
}

bool WinsenMHZ14::EnableAutomaticBaselineCorrection()
{
  return SetAutomaticBaselineCorrection(true);
}

// Automatic Baseline Correction (ABC function)
// ABC function refers to that sensor itself do zero point judgment and automatic calibration
// procedure intelligently after a continuous operation period. The automatic calibration cycle is
// every 24 hours after powered on. The zero point of automatic calibration is 400ppm. From
// July 2015, the default setting is with built-in automatic calibration function. To use the sensor
// better, the sensor must be placed in clean air
bool WinsenMHZ14::SetAutomaticBaselineCorrection(bool automaticBaselineCorrection)
{
  const uint8_t* command = automaticBaselineCorrection ? ENABLE_ABC : DISABLE_ABC;

  sl_status_t status = sl_iostream_write(_stream, command, 9);
  if (status != SL_STATUS_OK)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::SetAutomaticBaselineCorrection: sl_iostream_write returned status=%d.", status);
    return false;
  }

  // Wait a bit to allow sensor to respond before reading response
  vTaskDelay(pdMS_TO_TICKS(200));

  size_t bytesRead;
  status = sl_iostream_read(_stream, &_buffer, sizeof(_buffer), &bytesRead);
  if (status != SL_STATUS_OK)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::SetAutomaticBaselineCorrection: sl_iostream_read returned status=%d.", status);
    return false;
  }

  // Check expected response length
  if (bytesRead != 9)
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::SetAutomaticBaselineCorrection: Invalid response length, bytesRead=%d.", bytesRead);
    return false;
  }

  // Check the checksum
  if (!ChecksumOk(_buffer, bytesRead))
  {
    SILABS_LOG("[ERROR] WinsenMHZ14::SetAutomaticBaselineCorrection: Invalid checksum.");
    return false;
  }

  _automaticBaselineCorrection = automaticBaselineCorrection;

  return true;
}

uint8_t WinsenMHZ14::CalculateChecksum(uint8_t *buffer, int length)
{
  uint8_t checksum = 0;
  for(int i = 1; i < length - 1; i++)
  {
    checksum += buffer[i];
  }
  checksum = 0xff - checksum;
  checksum += 1;

  return checksum;
}

bool WinsenMHZ14::ChecksumOk(uint8_t *buffer, int length)
{
  uint8_t calculatedChecksum = CalculateChecksum(buffer, length);
  uint8_t checksum = buffer[length - 1];

  return (calculatedChecksum == checksum);
}

