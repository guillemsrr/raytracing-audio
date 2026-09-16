// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "ImpulseResponse.h"
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

        const ImpulseResponse& GetImpulseResponse() const
        {
            return _impulseResponse;
        }

    private:
        static constexpr float SpeedOfSound = 343.0f; // m/s

        Intersector _intersector;
        Propagator _propagator;

        const AudioEmitter* _emitter = nullptr;
        const Listener* _listener = nullptr;

        ImpulseResponse _impulseResponse;

        /** Converts a path length into an arrival time and adds the energy at that time. */
        void RecordArrival(float pathLength, const glm::vec3& energy);
    };
}
