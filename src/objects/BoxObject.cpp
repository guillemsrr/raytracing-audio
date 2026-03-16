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
    // Transform ray to local box space in double precision
    glm::dvec3 localOrigin = glm::dmat3(_invRotation) * (glm::dvec3(ray.origin()) - glm::dvec3(_center));
    glm::dvec3 localDir = glm::dmat3(_invRotation) * glm::dvec3(ray.direction());

    // Slab method for AABB in local space
    glm::dvec3 min = -_halfSize;
    glm::dvec3 max = _halfSize;

    double tmin = ray_t.min;
    double tmax = ray_t.max;

    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(localDir[i]) < 1e-12)
        {
            if (localOrigin[i] < min[i] || localOrigin[i] > max[i])
                return {};
        }
        else
        {
            double invD = 1.0 / localDir[i];
            double t0 = (min[i] - localOrigin[i]) * invD;
            double t1 = (max[i] - localOrigin[i]) * invD;

            if (invD < 0.0) std::swap(t0, t1);

            tmin = tmin > t0 ? tmin : t0;
            tmax = tmax < t1 ? tmax : t1;

            if (tmax <= tmin)
                return {};
        }
    }

    double t = tmin;

    HitResult hit;
    hit.t = t;
    glm::dvec3 localHitPoint = localOrigin + t * localDir;
    glm::dvec3 localNormal = glm::dvec3(0);

    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(localHitPoint[i] - max[i]) < 1e-6)
            localNormal[i] = 1.0;
        else if (std::abs(localHitPoint[i] - min[i]) < 1e-6)
            localNormal[i] = -1.0;
    }

    glm::dvec3 p_precise = glm::dvec3(ray.origin()) + t * glm::dvec3(ray.direction());
    hit.p = glm::vec3(p_precise);

    const glm::vec3 outwardNormal = glm::normalize(glm::vec3(glm::dmat3(_rotation) * localNormal));
    hit.SetObjectHit(this, ray, outwardNormal);

    return hit;
}