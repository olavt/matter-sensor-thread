/*
 * VEML6035AmbientLightSensor.h
 *
 *  Created on: Jul 3, 2024
 *      Author: olavt
 */

#ifndef INCLUDE_VEML6035AMBIENTLIGHTSENSOR_H_
#define INCLUDE_VEML6035AMBIENTLIGHTSENSOR_H_

class VEML6035AmbientLightSensor
{

private:

  bool _isInitialized = false;

  public:

    VEML6035AmbientLightSensor();

    bool Init();

    bool MeasureIllumination(float* illumination);

};

#endif /* INCLUDE_VEML6035AMBIENTLIGHTSENSOR_H_ */
