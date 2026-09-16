// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "raytracing/base/RaytracerBase.h"

namespace Light
{
    class WhittedTracer final : public RaytracerBase
    {
    public:
        explicit WhittedTracer(Camera* camera);

    protected:
        color TraceRay(const Ray& ray) const override;

    private:
        int _maxBounces = 4;

        color ShadeSurface(const HitResult& hit, const Ray& ray, int depth) const;
        color GetSkyColor(const glm::vec3& rayDirection) const;
    };
}
