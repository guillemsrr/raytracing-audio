// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "Propagator.h"
#include "raytracing/base/RaytracerBase.h"

class AudioEmitter;

namespace Audio
{
    /**
     * Pixel view of the sound field: colours every visible surface by how much acoustic
     *        energy reaches it from the emitter, per frequency band (low/mid/high -> R/G/B).
     *
     * Uses the light side's pixel machinery (RaytracerBase: camera rays, accumulation over frames)
     * with the audio side's physics (Propagator). By reciprocity the path runs backwards:
     * it starts at the surface point the pixel sees and targets the emitter, exactly as
     * Light::PathTracer starts at the camera and targets the sun.
     */
    class Visualizer final : public RaytracerBase
    {
    public:
        explicit Visualizer(Camera* camera);

        void SetEmitter(const AudioEmitter* emitter);

    protected:
        color TraceRay(const Ray& ray) const override;

        bool ShouldAccumulate() const override
        {
            return true;
        }

    private:
        const AudioEmitter* _emitter = nullptr;
        Propagator _propagator;
    };
}
