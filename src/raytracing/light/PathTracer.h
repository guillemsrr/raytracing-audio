// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "raytracing/base/RaytracerBase.h"

namespace Light
{
    class PathTracer : public RaytracerBase
    {
    public:
        explicit PathTracer(Camera* camera);

    protected:
        color TraceRay(const Ray& ray) const override;
        bool ShouldAccumulate() const override { return true; }

    private:
        int _maxBounces = 4;

        color ShadeSurface(const HitResult& hit, const Ray& ray) const;
        color GetSkyColor(const glm::vec3& rayDirection) const;
    };
}
