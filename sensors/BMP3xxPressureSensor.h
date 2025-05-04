/*
 * BMP3xxPressureSensor.h
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#pragma once

#include "BarometricPressureSensor.h"

class BMP3xxPressureSensor : public BarometricPressureSensor
{
private:
    bool _isInitialized = false;

public:
    BMP3xxPressureSensor() = default;

    bool Init() override;

    std::vector<Measurement> ReadAllMeasurements() override;

    std::optional<float> MeasureBarometricPressure() override;

};
