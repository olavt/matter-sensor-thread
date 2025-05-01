/*
 * BMP3xxPressureSensor.h
 *
 *  Created on: Jul 3, 2024
 *      Author: olavt
 */

#ifndef INCLUDE_BMP3XXPRESSURESENSOR_H_
#define INCLUDE_BMP3XXPRESSURESENSOR_H_

class BMP3xxPressureSensor
{

private:

  bool _isInitialized = false;

  public:

    BMP3xxPressureSensor();

    bool Init();

    bool MeasurePressure(float* pressure);

};

#endif /* INCLUDE_BMP3XXPRESSURESENSOR_H_ */
