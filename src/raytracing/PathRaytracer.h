// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "RaytracerBase.h"

class PathRaytracer : public RaytracerBase
{
public:
    explicit PathRaytracer(Camera* camera);

protected:
    color TraceRay(const Ray& ray) const override;
    bool ShouldAccumulate() const override { return true; }

private:
    int _maxBounces = 4;

    color ShadeSurface(const HitResult& hit, const Ray& ray) const;
    color GetSkyColor(const glm::vec3& rayDirection) const;
    bool IsOccluded(const glm::vec3& origin, const glm::vec3& direction) const;
};