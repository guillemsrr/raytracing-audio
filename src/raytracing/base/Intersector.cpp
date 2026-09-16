// Copyright (c) Guillem Serra. All Rights Reserved.

#include "Intersector.h"

#include "HitResult.h"
#include "Interval.h"
#include "Ray.h"
#include "scene/Scene.h"

Intersector::Intersector(float bias) : _bias(bias)
{
}

void Intersector::SetScene(const Scene& scene)
{
    _scene = &scene;
}

bool Intersector::HasScene() const
{
    return _scene != nullptr;
}

HitResult Intersector::ClosestHit(const Ray& ray, const float maxDistance) const
{
    if (!_scene)
        return HitResult();

    return _scene->HitAny(ray, Interval(_bias, maxDistance));
}

bool Intersector::IsOccluded(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const
{
    return ClosestHit(Ray(origin, direction), maxDistance - _bias).HasHit();
}

glm::vec3 Intersector::OffsetFromSurface(const HitResult& hit) const
{
    return hit.p + hit.normal * _bias;
}
