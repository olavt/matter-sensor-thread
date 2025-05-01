/*
 * I2SMicrophone.h
 *
 *  Created on: Nov 25, 2023
 *      Author: olavt
 */

#pragma once

class I2CMicrophone
{

private:

  bool _isInitialized = false;

  public:

  I2CMicrophone();

    bool Init();

    bool MeasureSoundLevel(float* soundLevel);

};

