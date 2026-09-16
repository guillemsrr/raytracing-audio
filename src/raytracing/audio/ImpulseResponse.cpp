// Copyright (c) Guillem Serra. All Rights Reserved.

#include "ImpulseResponse.h"

namespace Audio
{
    ImpulseResponse::ImpulseResponse()
    {
        Clear();
    }

    void ImpulseResponse::AddArrival(float arrivalTime, const glm::vec3& energy)
    {
        const auto timeIndex = static_cast<size_t>(arrivalTime / TimeResolution);
        if (timeIndex < Energy.size())
            Energy[timeIndex] += energy;
    }

    float ImpulseResponse::TimeAt(size_t i) const
    {
        return static_cast<float>(i) * TimeResolution;
    }

    float ImpulseResponse::TotalEnergyAt(size_t i) const
    {
        return Energy[i].x + Energy[i].y + Energy[i].z;
    }

    void ImpulseResponse::Clear()
    {
        Energy.assign(Duration / TimeResolution, glm::vec3(0.0f));
    }
}
