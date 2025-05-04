#include "AirQualitySensor.h"
#include <map>

bool AirQualitySensor::Init()
{
    SetSensorAltitude(m_sensorAltitude);

    return true;
}
