// Copyright (c) Guillem Serra. All Rights Reserved.

#include "Tracer.h"

#include "Listener.h"
#include "audio/AudioEmitter.h"
#include "utils/Utils.h"

namespace Audio
{
    Tracer::Tracer() = default;

    void Tracer::SetScene(const Scene& scene)
    {
        _intersector.SetScene(scene);
    }

    void Tracer::SetEmitter(const AudioEmitter* emitter)
    {
        _emitter = emitter;
    }

    void Tracer::SetListener(const Listener* listener)
    {
        _listener = listener;
    }

    void Tracer::Trace(int pathCount)
    {
        if (!_intersector.HasScene() || !_emitter || !_listener || pathCount <= 0)
            return;

        _impulseResponse.Clear();

        // The direct path does not depend on any random ray, so it is added exactly once.
        _propagator.PropagateDirect(_intersector,
                                    _emitter->Position,
                                    _listener->Position,
                                    [this](float pathLength, const glm::vec3& energy)
                                    {
                                        RecordArrival(pathLength, energy);
                                    });

        // Each random path is one sample of the reflected sound field; weight so the total
        // does not grow with pathCount (same idea as dividing the accumulation by _frameIndex).
        const float pathWeight = 1.0f / pathCount;
        const auto depositWeighted = [this, pathWeight](float pathLength, const glm::vec3& energy)
        {
            RecordArrival(pathLength, energy * pathWeight);
        };

        for (int i = 0; i < pathCount; ++i)
            _propagator.Propagate(_intersector,
                                  _emitter->Position,
                                  Utils::RandomUnitVector(),
                                  _listener->Position,
                                  depositWeighted);
    }

    void Tracer::RecordArrival(float pathLength, const glm::vec3& energy)
    {
        _impulseResponse.AddArrival(pathLength / SpeedOfSound, energy);
    }
}
