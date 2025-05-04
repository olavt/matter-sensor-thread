/*
 * MatterAirQuality.h
 *
 *  Created on: Apr 29, 2025
 *      Author: olavt
 */

#pragma once

#include <unordered_map>

#include <platform/CHIPDeviceLayer.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <platform/silabs/platformAbstraction/SilabsPlatform.h>
#include <app/clusters/air-quality-server/air-quality-server.h>
#include <app/clusters/concentration-measurement-server/concentration-measurement-server.h>

#include "AirQualitySensor.h"
#include "Measurements.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::AirQuality;
using namespace chip::DeviceLayer::Silabs;
using namespace chip::Protocols::InteractionModel;

class MatterAirQuality
{
    public:

        MatterAirQuality(EndpointId airQualityEndpointId, std::shared_ptr<AirQualitySensor> airQualitySensor);

        void StartMeasurements();

        void UpdateMeasurements();

    private:

        // Map from MeasurementType to Matter cluster ID
        static const std::unordered_map<AirQualitySensor::MeasurementType, uint32_t> measurementTypeToClusterId;

        // Reverse map from cluster ID to MeasurementType
        static const std::unordered_map<uint32_t, AirQualitySensor::MeasurementType> clusterIdToMeasurementType;

        static std::unordered_map<AirQualitySensor::MeasurementType, ConcentrationMeasurement::Instance<true, true, true, true, true, true>*> measurementTypeToInstance;

        EndpointId m_airQualityEndpointId;
        std::shared_ptr<AirQualitySensor> m_airQualitySensor;
        Measurements m_measurements;
        AirQuality::Instance* m_airQualityInstance;

        AirQualityEnum ClassifyAirQualityByCO2();
        AirQualityEnum ClassifyAirQualityByPM10();
        AirQualityEnum ClassifyAirQualityByPM25();
        AirQualityEnum ClassifyAirQuality();
        void CreateAirQualityInstance();

        void AddConcentrationMeasurementInstance(
            uint32_t clusterId,
            ConcentrationMeasurement::MeasurementMediumEnum medium,
            ConcentrationMeasurement::MeasurementUnitEnum unit,
            uint32_t averageWindowSizeSeconds,
            uint32_t peakWindowSizeSeconds);

        void AddConcentrationMeasurementInstances();

        void AddCarbonDioxideMeasurementInstance();
        void AddNitrogenDioxideMeasurementInstance();
        void AddPm1MeasurementInstance();
        void AddPm25MeasurementInstance();
        void AddPm10MeasurementInstance();
        void AddTotalVolatileOrganicCompoundsMeasurementInstance();

        void UpdateHumidityMeasuredValue(float relativeHumidity);

        void UpdateTemperatureMeasuredValue(float temperatureCelsius);

        static void UpdateAirQualityAttributes(intptr_t context);

};
