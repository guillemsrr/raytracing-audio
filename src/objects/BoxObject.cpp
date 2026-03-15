// Copyright (c) Guillem Serra. All Rights Reserved.

#include "BoxObject.h"

#include <algorithm>

#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "raytracing/HitResult.h"
#include "raytracing/Interval.h"
#include "raytracing/Ray.h"

BoxObject::BoxObject(const glm::vec3& center, const glm::vec3& size) : ObjectBase(center),
                                                                       _invRotation(glm::transpose(_rotation))
{
    _halfSize = size * 0.5f;
}

BoxObject::BoxObject(const glm::vec3& center, const glm::vec3& size, const glm::vec3& rotation) : ObjectBase(center)
{
    _halfSize = size * 0.5f;
    _rotation = glm::mat3(glm::yawPitchRoll(glm::radians(rotation.x), glm::radians(rotation.y),
                                            glm::radians(rotation.z)));
    _invRotation = glm::transpose(_rotation);
}

HitResult BoxObject::HitInRayInterval(Ray ray, Interval ray_t) const
{
    // Transform ray to local box space
    glm::vec3 localOrigin = _invRotation * (ray.origin() - _center);
    glm::vec3 localDir = _invRotation * ray.direction();

    // Slab method for AABB in local space
    glm::vec3 min = -_halfSize;
    glm::vec3 max = _halfSize;

    float tmin = ray_t.min;
    float tmax = ray_t.max;

    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(localDir[i]) < 1e-6f)
        {
            if (localOrigin[i] < min[i] || localOrigin[i] > max[i])
                return {};
        }
        else
        {
            float invD = 1.0f / localDir[i];
            float t0 = (min[i] - localOrigin[i]) * invD;
            float t1 = (max[i] - localOrigin[i]) * invD;

            if (invD < 0.0f) std::swap(t0, t1);

            tmin = std::max(tmin, t0);
            tmax = std::min(tmax, t1);

            if (tmax < tmin)
                return {};
        }
    }

    float t = tmin;

    HitResult hit;
    hit.t = t;
    glm::vec3 localHitPoint = localOrigin + t * localDir;
    glm::vec3 localNormal = glm::vec3(0);

    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(localHitPoint[i] - max[i]) < 1e-3f)
            localNormal[i] = 1.0f;
        else if (std::abs(localHitPoint[i] - min[i]) < 1e-3f)
            localNormal[i] = -1.0f;
    }

    // Transform back to world space
    hit.p = ray.at(t);
    const glm::vec3 outwardNormal = glm::normalize(_rotation * localNormal);
    hit.SetObjectHit(this, ray, outwardNormal);

    return hit;
}