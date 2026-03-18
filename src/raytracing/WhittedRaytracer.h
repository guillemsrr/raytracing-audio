// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "base/RaytracerBase.h"

class WhittedRaytracer : public RaytracerBase
{
public:
    explicit WhittedRaytracer(Camera* camera);

protected:
    color TraceRay(const Ray& ray) const override;

private:
    int _maxBounces = 4;

    color ShadeSurface(const HitResult& hit, const Ray& ray, int depth) const;
    color GetSkyColor(const glm::vec3& rayDirection) const;
    bool IsOccluded(const glm::vec3& origin, const glm::vec3& direction) const;
};
