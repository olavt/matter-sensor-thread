/*
 * I2SMicrophone.cpp
 *
 *  Created on: Nov 25, 2023
 *      Author: olavt
 */


#include <I2CMicrophone.h>
#include "sl_mic.h"
#include "silabs_utils.h"

#define MIC_SAMPLE_RATE           44100
#define MIC_N_SAMPLES             1024
#define MIC_N_CHANNELS            2

static int16_t buffer[MIC_N_SAMPLES * MIC_N_CHANNELS];

I2CMicrophone::I2CMicrophone()
{
}

bool I2CMicrophone::Init()
{
  if (_isInitialized)
    return true;

  // Initialize microphone with sample rate and number of channels
  sl_status_t status = sl_mic_init(MIC_SAMPLE_RATE, MIC_N_CHANNELS);

  _isInitialized = (status == SL_STATUS_OK);
  if (status != SL_STATUS_OK)
    SILABS_LOG("[ERROR] I2CMicrophone.Init: Initialization failed with status=%d", status);

  return _isInitialized;
}

bool I2CMicrophone::MeasureSoundLevel(float* soundLevel)
{
  float sound_level_0;
  float sound_level_1;

  // Read samples from the microphone
  sl_mic_get_n_samples(buffer, MIC_N_SAMPLES);

  while (!sl_mic_sample_buffer_ready()) {
    // Wait until sample buffer ready
  }

  // Calculate sound level
  sl_mic_calculate_sound_level(&sound_level_0, buffer, MIC_N_SAMPLES, 0);
  sl_mic_calculate_sound_level(&sound_level_1, buffer, MIC_N_SAMPLES, 1);

  *soundLevel = (sound_level_0 + sound_level_1) / 2;

  return true;
}

