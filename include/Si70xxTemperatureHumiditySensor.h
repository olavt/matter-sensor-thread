/*
 * Si70xxTemperatureHumiditySensor.h
 *
 *  Created on: Jul 3, 2024
 *      Author: olavt
 */

#ifndef INCLUDE_SI70XXTEMPERATUREHUMIDITYSENSOR_H_
#define INCLUDE_SI70XXTEMPERATUREHUMIDITYSENSOR_H_

class Si70xxTemperatureHumiditySensor
{

private:

  bool _isInitialized = false;

  public:

    Si70xxTemperatureHumiditySensor();

    bool Init();

    bool MeasureRelativeHumidity(float* relativeHumidity);

    bool MeasureTemperature(float* temperature);

};

#endif /* INCLUDE_SI70XXTEMPERATUREHUMIDITYSENSOR_H_ */
