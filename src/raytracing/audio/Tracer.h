// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "Propagator.h"
#include "raytracing/base/Intersector.h"

class AudioEmitter;
class Scene;

namespace Audio
{
    class Listener;

    class Tracer
    {
    public:
        Tracer();

        void SetScene(const Scene& scene);
        void SetEmitter(const AudioEmitter* emitter);
        void SetListener(const Listener* listener);

        /**
         * Clears the impulse response, adds the direct path once, then traces pathCount random paths.
         */
        void Trace(int pathCount);

        const std::vector<glm::vec3>& GetImpulseResponse() const
        {
            return _impulseResponse;
        }

        float GetTimeResolution() const
        {
            return _timeResolution;
        }

        float GetDuration() const
        {
            return _duration;
        }

    private:
        static constexpr float SpeedOfSound = 343.0f; // m/s

        Intersector _intersector;
        Propagator _propagator;

        const AudioEmitter* _emitter = nullptr;
        const Listener* _listener = nullptr;

        float _timeResolution = 0.001f; // seconds per entry of the impulse response (1 ms)
        float _duration = 2.f; // seconds covered by the impulse response

        // Analogue of _colorBuffer: energy per band, indexed by arrival time instead of by pixel.
        std::vector<glm::vec3> _impulseResponse;

        /** Converts a path length into an arrival time and adds the energy at that time. */
        void RecordArrival(float pathLength, const glm::vec3& energy);
    };
}
