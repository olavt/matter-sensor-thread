#pragma once

#include "sensor.h"


class AirQualitySensor : public Sensor
{

public:


    // Constructor
    explicit AirQualitySensor(float sensorAltitude = 0.0f)
        : m_sensorAltitude(sensorAltitude)
    {
    }

    // Virtual destructor for proper cleanup
    virtual ~AirQualitySensor() = default;

    // Initialize the sensor
    bool Init() override;

    // Get the set of measurement types supported by this sensor
    virtual std::set<MeasurementType> GetSupportedMeasurements() const = 0;

    // Start continuous measurement mode (if supported)
    // Returns 0 on success, non-zero on failure
    virtual int StartContinuousMeasurement() = 0;

protected:
    // Set sensor altitude (protected, as it’s implementation-specific)
    virtual int SetSensorAltitude(float sensorAltitude) = 0;

    float m_sensorAltitude;
};
