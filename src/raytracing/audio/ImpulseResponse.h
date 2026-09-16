// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include <vector>
#include <glm/vec3.hpp>

namespace Audio
{
    /**
     * @brief What a listener receives after the emitter produces a single instantaneous "click":
     *        energy per frequency band (low/mid/high) at each moment in time.
     *
     * Analogue of the pixel buffer: indexed by arrival time instead of by pixel.
     * Convolving a source signal with this is what makes the listener "hear the room".
     */
    struct ImpulseResponse
    {
        std::vector<glm::vec3> Energy;
        float TimeResolution = 0.001f;
        float Duration = 2.0f;

        ImpulseResponse();

        void AddArrival(float arrivalTime, const glm::vec3& energy);

        float TimeAt(size_t i) const;

        float TotalEnergyAt(size_t i) const;
        void Clear();
    };
}
