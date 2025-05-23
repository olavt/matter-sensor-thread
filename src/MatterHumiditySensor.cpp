#include <cmath>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <MatterHumiditySensor.h>
#include "silabs_utils.h"

MatterHumiditySensor::MatterHumiditySensor(EndpointId humidityEndpointId, std::shared_ptr<RelativeHumiditySensor> humiditySensor)
: m_humidityEndpointId(humidityEndpointId), m_humiditySensor(humiditySensor)
{
}

void MatterHumiditySensor::UpdateMeasurements()
{
  auto humidity = m_humiditySensor->MeasureRelativeHumidity();
  if (humidity)
    {
      SILABS_LOG("[INFO] Updating humidity measurement.");

      UpdateHumidityAttributes(m_humidityEndpointId, humidity.value());
    }
}

void MatterHumiditySensor::UpdateHumidityAttributes(EndpointId endpoint, float relativeHumidity)
{
  SILABS_LOG("[INFO] UpdateHumidityMeasuredValue: relativeHumidity=%f", relativeHumidity);

  // Convert relativeHumidity (in percent) to hundredths of a percent (uint16_t)
  float scaledValue = relativeHumidity * 100.0f; // Scale to hundredths of a percent

  // Round to nearest integer for accuracy
  float roundedValue = std::round(scaledValue);

  // Convert to uint16_t
  uint16_t newValue = static_cast<uint16_t>(roundedValue);

  chip::app::Clusters::RelativeHumidityMeasurement::Attributes::MeasuredValue::Set(endpoint, newValue);
}



