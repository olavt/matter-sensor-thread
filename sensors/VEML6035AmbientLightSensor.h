/*
 * VEML6035AmbientLightSensor.h
 *
 *  Created on: May 4, 2025
 *      Author: olavt
 */

#pragma once

#include "AmbientLightSensor.h"

class VEML6035AmbientLightSensor : public AmbientLightSensor
{
private:
    bool _isInitialized = false;

public:
    VEML6035AmbientLightSensor() = default;

    bool Init() override;

    std::vector<Measurement> ReadAllMeasurements() override;

    std::optional<float> MeasureAmbientLight() override;

};
