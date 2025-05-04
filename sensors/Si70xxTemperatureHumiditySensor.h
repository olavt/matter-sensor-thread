/*
 * Si70xxTemperatureHumiditySensor.h
 *
 *  Created on: May 3, 2025
 *      Author: olavt
 */

#pragma once

#include "TemperatureSensor.h"
#include "RelativeHumiditySensor.h"

class Si70xxTemperatureHumiditySensor : public TemperatureSensor, public RelativeHumiditySensor {
private:
    bool _isInitialized = false;

public:
    Si70xxTemperatureHumiditySensor() = default;

    bool Init() override;

    std::vector<Measurement> ReadAllMeasurements() override;

    std::optional<float> MeasureRelativeHumidity() override;

    std::optional<float> MeasureTemperature() override;

};
