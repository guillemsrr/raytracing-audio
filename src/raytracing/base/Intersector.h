// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include <cfloat>
#include <glm/vec3.hpp>

class Scene;
class Ray;
struct HitResult;

class Intersector
{
public:
    explicit Intersector(float bias = 0.02f);

    void SetScene(const Scene& scene);

    bool HasScene() const;

    HitResult ClosestHit(const Ray& ray, float maxDistance = FLT_MAX) const;

    bool IsOccluded(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = FLT_MAX) const;

    glm::vec3 OffsetFromSurface(const HitResult& hit) const;

    float GetBias() const
    {
        return _bias;
    }

private:
    const Scene* _scene = nullptr;
    float _bias;
};
