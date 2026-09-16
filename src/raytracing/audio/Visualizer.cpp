// Copyright (c) Guillem Serra. All Rights Reserved.

#include "Visualizer.h"

#include "audio/AudioEmitter.h"
#include "raytracing/base/HitResult.h"
#include "utils/Utils.h"

namespace Audio
{
    Visualizer::Visualizer(Camera* camera) : RaytracerBase(camera)
    {
    }

    void Visualizer::SetEmitter(const AudioEmitter* emitter)
    {
        _emitter = emitter;
    }

    color Visualizer::TraceRay(const Ray& ray) const
    {
        if (!_emitter)
        {
            return Colors::Black;
        }

        const HitResult hit = _intersector.ClosestHit(ray);
        if (!hit.HasHit())
        {
            return Colors::Black;
        }

        const glm::vec3 surfacePoint = _intersector.OffsetFromSurface(hit);
        glm::vec3 energy(0.0f);
        const auto accumulate = [&energy](float /*pathLength*/, const glm::vec3& arrived)
        {
            energy += arrived;
        };

        _propagator.PropagateDirect(_intersector, surfacePoint, _emitter->Position, accumulate);

        const glm::vec3 bounceDirection = glm::normalize(hit.normal + Utils::RandomUnitVector());
        _propagator.Propagate(_intersector, surfacePoint, bounceDirection, _emitter->Position, accumulate);

        return color(energy, 1.0f);
    }
}
