// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include <functional>
#include <glm/vec3.hpp>

class Intersector;

namespace Audio
{
    /**
     * Propagates sound through the scene: the acoustic twin of Light::PathTracer::TraceRay's loop.
     *
     * Walks from an origin, bouncing off surfaces, and at every bounce connects explicitly to a
     * target point (next event estimation). Each connection reports how far the sound travelled
     * and how much energy per frequency band (low/mid/high) survived, via a callback.
     *
     * It does not know what the caller does with an arrival, so the same loop serves both:
     *  - the impulse response  (origin = emitter,  target = listener, arrivals -> time histogram)
     *  - a pixel view          (origin = surface,  target = emitter,  arrivals -> pixel colour)
     * Physics is reciprocal, so propagating in either direction gives the same energy.
     */
    class Propagator
    {
    public:
        using ArrivalCallback = std::function<void(float pathLength, const glm::vec3& energy)>;

        int MaxBounces = 10;
        float MaxRayDistance = 100000.0f;

        /** Straight line origin -> target, no bounces. Deterministic: call it once, not per path. */
        void PropagateDirect(const Intersector& intersector,
                             const glm::vec3& origin,
                             const glm::vec3& target,
                             const ArrivalCallback& onArrival) const;

        /** One random path from origin along direction; reports an arrival at every bounce that can see the target. */
        void Propagate(const Intersector& intersector,
                       const glm::vec3& origin,
                       const glm::vec3& direction,
                       const glm::vec3& target,
                       const ArrivalCallback& onArrival) const;
    };
}
