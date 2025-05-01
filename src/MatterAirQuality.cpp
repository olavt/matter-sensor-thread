/*
 * MatterAirQuality.cpp
 *
 *  Created on: Apr 29, 2025
 *      Author: olavt
 */

#include "MatterAirQuality.h"

#include "silabs_utils.h"

#include <platform/CHIPDeviceLayer.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <platform/silabs/platformAbstraction/SilabsPlatform.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::DataModel;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::AirQuality;

// Initialize measurementTypeToClusterId
const std::unordered_map<AirQualitySensor::MeasurementType, uint32_t> MatterAirQuality::measurementTypeToClusterId = {
    {AirQualitySensor::MeasurementType::AmbientHumidity, RelativeHumidityMeasurement::Id},
    {AirQualitySensor::MeasurementType::AmbientTemperature, TemperatureMeasurement::Id},
    {AirQualitySensor::MeasurementType::CO2, CarbonDioxideConcentrationMeasurement::Id},
    {AirQualitySensor::MeasurementType::NOxIndex, NitrogenDioxideConcentrationMeasurement::Id},
    {AirQualitySensor::MeasurementType::VOCIndex, TotalVolatileOrganicCompoundsConcentrationMeasurement::Id},
    {AirQualitySensor::MeasurementType::ParticulateMatter1p0, Pm1ConcentrationMeasurement::Id},
    {AirQualitySensor::MeasurementType::ParticulateMatter2p5, Pm25ConcentrationMeasurement::Id},
    {AirQualitySensor::MeasurementType::ParticulateMatter10p0, Pm10ConcentrationMeasurement::Id}
};

// Initialize clusterIdToMeasurementType (reverse mapping)
const std::unordered_map<uint32_t, AirQualitySensor::MeasurementType> MatterAirQuality::clusterIdToMeasurementType = {
    {RelativeHumidityMeasurement::Id, AirQualitySensor::MeasurementType::AmbientHumidity},
    {TemperatureMeasurement::Id, AirQualitySensor::MeasurementType::AmbientTemperature},
    {CarbonDioxideConcentrationMeasurement::Id, AirQualitySensor::MeasurementType::CO2},
    {NitrogenDioxideConcentrationMeasurement::Id, AirQualitySensor::MeasurementType::NOxIndex},
    {TotalVolatileOrganicCompoundsConcentrationMeasurement::Id, AirQualitySensor::MeasurementType::VOCIndex},
    {Pm1ConcentrationMeasurement::Id, AirQualitySensor::MeasurementType::ParticulateMatter1p0},
    {Pm25ConcentrationMeasurement::Id, AirQualitySensor::MeasurementType::ParticulateMatter2p5},
    {Pm10ConcentrationMeasurement::Id, AirQualitySensor::MeasurementType::ParticulateMatter10p0}
};

std::unordered_map<AirQualitySensor::MeasurementType, ConcentrationMeasurement::Instance<true, true, true, true, true, true>*> MatterAirQuality::measurementTypeToInstance;

// Returns the elapsed seconds since boot of the device
float getElapsedSeconds()
{
  // Get the current monotonic time in milliseconds
  uint64_t tick_ms = chip::System::SystemClock().GetMonotonicMilliseconds64().count();

  // Convert milliseconds to seconds
  float elapsedSeconds = static_cast<float>(tick_ms) / 1000.0f;

  return elapsedSeconds;
}

MatterAirQuality::MatterAirQuality(std::unique_ptr<AirQualitySensor> airQualitySensor, EndpointId airQualityEndpointId)
    : m_airQualitySensor(std::move(airQualitySensor)), m_airQualityEndpointId(airQualityEndpointId)
{
    CreateAirQualityInstance();
    AddConcentrationMeasurementInstances();
}

void MatterAirQuality::StartMeasurements()
{
    m_airQualitySensor->StartContinuousMeasurement();
}

void MatterAirQuality::CreateAirQualityInstance()
{
  m_measurements.AddId(RelativeHumidityMeasurement::Id, 60, 60);
  m_measurements.AddId(TemperatureMeasurement::Id, 60, 60);

  BitMask<AirQuality::Feature> features = BitMask<AirQuality::Feature>(
      AirQuality::Feature::kFair,
      AirQuality::Feature::kModerate,
      AirQuality::Feature::kVeryPoor,
      AirQuality::Feature::kExtremelyPoor);

  m_airQualityInstance = new AirQuality::Instance(
      m_airQualityEndpointId,
      features
      );

  m_airQualityInstance->Init();
}

void MatterAirQuality::AddConcentrationMeasurementInstance(
    uint32_t clusterId,
    ConcentrationMeasurement::MeasurementMediumEnum medium,
    ConcentrationMeasurement::MeasurementUnitEnum unit,
    uint32_t averageWindowSizeSeconds,
    uint32_t peakWindowSizeSeconds)
{
    auto typeIt = clusterIdToMeasurementType.find(clusterId);
    if (typeIt == clusterIdToMeasurementType.end()) {
        SILABS_LOG("AddMeasurementInstance: No MeasurementType found for cluster %u", clusterId);
        return;
    }
    AirQualitySensor::MeasurementType measurementType = typeIt->second;

    m_measurements.AddId(clusterId, averageWindowSizeSeconds, peakWindowSizeSeconds);

    auto* instance = new ConcentrationMeasurement::Instance<true, true, true, true, true, true>(
        m_airQualityEndpointId, clusterId, medium, unit);

    if (instance == nullptr) {
        SILABS_LOG("AddMeasurementInstance: Failed to allocate instance for type %s",
                   AirQualitySensor::MeasurementTypeToString(measurementType).c_str());
        return;
    }

    CHIP_ERROR err = instance->Init();
    if (err != CHIP_NO_ERROR) {
        SILABS_LOG("AddMeasurementInstance: Failed to initialize instance for type %s: %s",
                   AirQualitySensor::MeasurementTypeToString(measurementType).c_str(), err.AsString());
        delete instance;
        return;
    }

    measurementTypeToInstance.emplace(measurementType, instance);
}

void MatterAirQuality::AddCarbonDioxideMeasurementInstance()
{
    AddConcentrationMeasurementInstance(
        CarbonDioxideConcentrationMeasurement::Id,
        ConcentrationMeasurement::MeasurementMediumEnum::kAir,
        ConcentrationMeasurement::MeasurementUnitEnum::kPpm,
        3600,
        3600
        );
}

void MatterAirQuality::AddNitrogenDioxideMeasurementInstance()
{
  AddConcentrationMeasurementInstance(
      NitrogenDioxideConcentrationMeasurement::Id,
      ConcentrationMeasurement::MeasurementMediumEnum::kAir,
      ConcentrationMeasurement::MeasurementUnitEnum::kPpm,
      3600,
      3600
      );
}

void MatterAirQuality::AddPm1MeasurementInstance()
{
  AddConcentrationMeasurementInstance(
      Pm1ConcentrationMeasurement::Id,
      ConcentrationMeasurement::MeasurementMediumEnum::kAir,
      ConcentrationMeasurement::MeasurementUnitEnum::kUgm3,
      3600,
      3600
      );
  }

void MatterAirQuality::AddPm25MeasurementInstance()
{
  AddConcentrationMeasurementInstance(
      Pm25ConcentrationMeasurement::Id,
      ConcentrationMeasurement::MeasurementMediumEnum::kAir,
      ConcentrationMeasurement::MeasurementUnitEnum::kUgm3,
      3600,
      3600
      );
}

void MatterAirQuality::AddPm10MeasurementInstance()
{
  AddConcentrationMeasurementInstance(
      Pm10ConcentrationMeasurement::Id,
      ConcentrationMeasurement::MeasurementMediumEnum::kAir,
      ConcentrationMeasurement::MeasurementUnitEnum::kUgm3,
      3600,
      3600
      );
}

void MatterAirQuality::AddTotalVolatileOrganicCompoundsMeasurementInstance()
{
  AddConcentrationMeasurementInstance(
      TotalVolatileOrganicCompoundsConcentrationMeasurement::Id,
      ConcentrationMeasurement::MeasurementMediumEnum::kAir,
      ConcentrationMeasurement::MeasurementUnitEnum::kPpm,
      3600,
      3600
      );
}

void MatterAirQuality::AddConcentrationMeasurementInstances()
{
  // Add Concentration Measurement Clusters
  std::set<AirQualitySensor::MeasurementType> supportedMeasurements = m_airQualitySensor->GetSupportedMeasurements();

  // Add Concentration Measurement Instances based on supported measurements

  if (supportedMeasurements.count(AirQualitySensor::MeasurementType::CO2)) {
      AddCarbonDioxideMeasurementInstance();
  }

  if (supportedMeasurements.count(AirQualitySensor::MeasurementType::NOxIndex)) {
      AddNitrogenDioxideMeasurementInstance();
  }

  if (supportedMeasurements.count(AirQualitySensor::MeasurementType::ParticulateMatter1p0)) {
      AddPm1MeasurementInstance();
  }

  if (supportedMeasurements.count(AirQualitySensor::MeasurementType::ParticulateMatter2p5)) {
      AddPm25MeasurementInstance();
  }

  if (supportedMeasurements.count(AirQualitySensor::MeasurementType::ParticulateMatter10p0)) {
      AddPm10MeasurementInstance();
  }

  if (supportedMeasurements.count(AirQualitySensor::MeasurementType::VOCIndex)) {
      AddTotalVolatileOrganicCompoundsMeasurementInstance();
  }

}

AirQualityEnum MatterAirQuality::ClassifyAirQualityByCO2()
{
  uint16_t co2_ppm = m_measurements.GetLatest(CarbonDioxideConcentrationMeasurement::Id);

  if (co2_ppm >= 400 && co2_ppm <= 600) {
      // Fresh air, no noticeable effects; matches outdoor levels.
      return AirQualityEnum::kGood;
  } else if (co2_ppm <= 700) {
      // Still very good, no perceptible impact; minor ventilation decline.
      return AirQualityEnum::kFair;
  } else if (co2_ppm <= 800) {
      // Suboptimal; sensitive individuals might notice slight stuffiness.
      return AirQualityEnum::kModerate;
  } else if (co2_ppm <= 950) {
      // Mild effects possible (e.g., reduced focus); ventilation clearly poor.
      return AirQualityEnum::kPoor;
  } else if (co2_ppm <= 1200) {
      // Discomfort likely (e.g., stuffiness, drowsiness); significant air quality decline.
      return AirQualityEnum::kVeryPoor;
  } else if (co2_ppm > 1200) {
      // Potential health impacts (e.g., fatigue, headaches); unacceptable levels.
      return AirQualityEnum::kExtremelyPoor;
  }
  // Below 400 ppm or invalid readings; sensor error or uninitialized state.
  return AirQualityEnum::kUnknown;
}

AirQualityEnum MatterAirQuality::ClassifyAirQualityByPM10()
{
    uint16_t pm10 = m_measurements.GetAverage(Pm10ConcentrationMeasurement::Id);

    if (pm10 <= 30.0) {
        return AirQualityEnum::kGood;
    } else if (pm10 <= 60.0) {
        return AirQualityEnum::kFair;
    } else if (pm10 <= 120.0) {
        return AirQualityEnum::kModerate;
    } else if (pm10 <= 260.0) {
        return AirQualityEnum::kPoor;
    } else if (pm10 <= 400) {
        return AirQualityEnum::kVeryPoor;
    } else {
        return AirQualityEnum::kExtremelyPoor;
    }
}

AirQualityEnum MatterAirQuality::ClassifyAirQualityByPM25()
{
    uint16_t pm25 = m_measurements.GetAverage(Pm25ConcentrationMeasurement::Id);

    if (pm25 <= 15.0) {
        return AirQualityEnum::kGood;
    } else if (pm25 <= 30.0) {
        return AirQualityEnum::kFair;
    } else if (pm25 <= 50.0) {
        return AirQualityEnum::kModerate;
    } else if (pm25 <= 100.0) {
        return AirQualityEnum::kPoor;
    } else if (pm25 <= 150.0) {
        return AirQualityEnum::kVeryPoor;
    } else {
        return AirQualityEnum::kExtremelyPoor;
    }
}

AirQualityEnum MatterAirQuality::ClassifyAirQuality()
{
  // Classify air quality based on the measured substances

  AirQualityEnum airQuality = AirQuality::AirQualityEnum::kGood;

  std::vector<uint32_t> clusterIds = m_measurements.GetIds();
  for (uint32_t clusterId : clusterIds) {
      if (clusterId == CarbonDioxideConcentrationMeasurement::Id)
          airQuality = std::max(airQuality, ClassifyAirQualityByCO2());
      else if (clusterId == Pm25ConcentrationMeasurement::Id)
          airQuality = std::max(airQuality, ClassifyAirQualityByPM25());
      else if (clusterId == Pm10ConcentrationMeasurement::Id)
          airQuality = std::max(airQuality, ClassifyAirQualityByPM10());
  }

  return airQuality;
}

void MatterAirQuality::MeasureAirQuality()
{
    // Read all measurements from the sensor
    std::vector<AirQualitySensor::Measurement> measurements = m_airQualitySensor->ReadAllMeasurements();

    // Check if measurements are empty (indicating an error)
    if (measurements.empty()) {
        //ESP_LOGE(TAG, "MeasureAirQuality: sensor->ReadAllMeasurements failed or returned no data");
        return;
    }

    // Process each measurement
    for (const auto& measurement : measurements) {

        // Look up the cluster ID for the measurement type
        auto it = measurementTypeToClusterId.find(measurement.type);

            // Check if the cluster ID was found
        if (it == measurementTypeToClusterId.end()) {
            // Log an error and skip this measurement
            SILABS_LOG("MeasureAirQuality: No cluster ID found for measurement type %s",
                       AirQualitySensor::MeasurementTypeToString(measurement.type).c_str());
            continue; // Skip to the next measurement
        }

        uint32_t clusterId = it->second;

        // Log the measurement
        SILABS_LOG("MeasureAirQuality: %s: %f",
                    AirQualitySensor::MeasurementTypeToString(measurement.type).c_str(),
                    measurement.value);

        float elapsedSeconds = getElapsedSeconds();

        // Add the measurement to the measurements store
        m_measurements.AddMeasurement(clusterId, measurement.value, elapsedSeconds);
    }

    DeviceLayer::PlatformMgr().ScheduleWork(UpdateAirQualityAttributes, reinterpret_cast<intptr_t>(this));
}

void MatterAirQuality::UpdateAirQualityAttributes(intptr_t context)
{
    MatterAirQuality* matterAirQuality = reinterpret_cast<MatterAirQuality*>(context);
    std::vector<uint32_t> clusterIds = matterAirQuality->m_measurements.GetIds();
    for (uint32_t clusterId : clusterIds) {
        if (clusterId == RelativeHumidityMeasurement::Id)
        {
            matterAirQuality->UpdateHumidityMeasuredValue(matterAirQuality->m_measurements.GetLatest(clusterId));
        }
        else if (clusterId == TemperatureMeasurement::Id)
        {
            matterAirQuality->UpdateTemperatureMeasuredValue(matterAirQuality->m_measurements.GetLatest(clusterId));
        }
        else
        {
            // Handle Concentration Measurement clusters
            auto typeIt = clusterIdToMeasurementType.find(clusterId);
            if (typeIt == clusterIdToMeasurementType.end()) {
                SILABS_LOG("UpdateAirQualityAttributes: No MeasurementType found for cluster %u", clusterId);
                continue;
            }

            auto instanceIt = measurementTypeToInstance.find(typeIt->second);
            if (instanceIt == measurementTypeToInstance.end()) {
                SILABS_LOG("UpdateAirQualityAttributes: No instance found for MeasurementType %s",
                           AirQualitySensor::MeasurementTypeToString(typeIt->second).c_str());
                continue;
            }

            auto instance = instanceIt->second;

            // Update MeasuredValue
            instance->SetMeasuredValue(MakeNullable(matterAirQuality->m_measurements.GetLatest(clusterId)));

            // Update AverageMeasuredValue
            instance->SetAverageMeasuredValue(MakeNullable(matterAirQuality->m_measurements.GetAverage(clusterId)));

            // Update PeakMeasuredValue
            instance->SetPeakMeasuredValue(MakeNullable(matterAirQuality->m_measurements.GetPeak(clusterId)));
        }
    }

    // Assess the overall air quality based on the measured substances
    AirQualityEnum airQuality = matterAirQuality->ClassifyAirQuality();

    // Update the AirQuality attribute
    matterAirQuality->m_airQualityInstance->UpdateAirQuality(airQuality);

}

void MatterAirQuality::UpdateHumidityMeasuredValue(float relativeHumidity)
{
  SILABS_LOG("[INFO] UpdateHumidityMeasuredValue: relativeHumidity=%f", relativeHumidity);
  chip::app::Clusters::RelativeHumidityMeasurement::Attributes::MeasuredValue::Set(m_airQualityEndpointId, relativeHumidity * 100);
}

void MatterAirQuality::UpdateTemperatureMeasuredValue(float temperatureCelsius)
{
  int16_t reportedTemperature = (temperatureCelsius * 100 + 0.5);
  SILABS_LOG("[INFO] UpdateTemperatureMeasuredValue: reportedTemperature=%d", reportedTemperature);
  chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Set(m_airQualityEndpointId, reportedTemperature);
}
