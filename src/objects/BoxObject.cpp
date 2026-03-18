// Copyright (c) Guillem Serra. All Rights Reserved.

#include "BoxObject.h"

#include <algorithm>

#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "../raytracing/base/HitResult.h"
#include "raytracing/base/Interval.h"
#include "../raytracing/base/Ray.h"

BoxObject::BoxObject(const glm::vec3& center, const glm::vec3& size) : ObjectBase(center)
{
    _halfSize = size * 0.5f;
}

BoxObject::BoxObject(const glm::vec3& center, const glm::vec3& size, const glm::vec3& rotation) : ObjectBase(center)
{
    _halfSize = size * 0.5f;
    Rotation = glm::quat(glm::radians(rotation));
}

HitResult BoxObject::HitInRayInterval(Ray ray, Interval ray_t) const
{
    // Transform ray to local box space in double precision
    glm::dmat3 invRot = glm::inverse(glm::mat3_cast(Rotation));
    glm::dvec3 localOrigin = invRot * (glm::dvec3(ray.origin()) - glm::dvec3(Position));
    glm::dvec3 localDir = invRot * glm::dvec3(ray.direction());

    // Slab method for AABB in local space
    glm::dvec3 minBounds = -glm::dvec3(_halfSize);
    glm::dvec3 maxBounds = glm::dvec3(_halfSize);

    double tmin = ray_t.min;
    double tmax = ray_t.max;
    int hit_axis = -1;
    double hit_sign = 0.0;

    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(localDir[i]) < 1e-12)
        {
            if (localOrigin[i] < minBounds[i] || localOrigin[i] > maxBounds[i])
                return {};
        }
        else
        {
            double invD = 1.0 / localDir[i];
            double t0 = (minBounds[i] - localOrigin[i]) * invD;
            double t1 = (maxBounds[i] - localOrigin[i]) * invD;

            if (invD < 0.0) std::swap(t0, t1);

            if (t0 > tmin)
            {
                tmin = t0;
                hit_axis = i;
                hit_sign = (invD < 0.0) ? 1.0 : -1.0;
            }
            if (t1 < tmax) tmax = t1;

            if (tmax <= tmin)
                return {};
        }
    }

    // If hit_axis remains -1, either the ray started inside or it didn't hit from outside.
    if (hit_axis == -1)
    {
        return {};
    }

    HitResult hit;
    hit.t = tmin;
    glm::dvec3 p_precise = glm::dvec3(ray.origin()) + tmin * glm::dvec3(ray.direction());
    hit.p = glm::vec3(p_precise);

    glm::dvec3 localNormal(0.0);
    localNormal[hit_axis] = hit_sign;

    const glm::vec3 outwardNormal = glm::normalize(glm::mat3_cast(Rotation) * glm::vec3(localNormal));
    hit.SetObjectHit(this, ray, outwardNormal);

    return hit;
}
