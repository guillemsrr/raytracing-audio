// Copyright (c) Guillem Serra. All Rights Reserved.

#include "BoxObject.h"

#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "../raytracing/base/HitResult.h"
#include "raytracing/base/Interval.h"
#include "../raytracing/base/Ray.h"

BoxObject::BoxObject(const vec3& center, const vec3& size) : ObjectBase(center)
{
    _halfSize = size * 0.5f;
}

BoxObject::BoxObject(const vec3& center, const vec3& size, const vec3& rotation) : ObjectBase(center)
{
    _halfSize = size * 0.5f;
    Rotation = glm::quat(glm::radians(rotation));
}

HitResult BoxObject::HitInRayInterval(Ray ray, Interval ray_t) const
{
    glm::dmat3 invRot = glm::inverse(glm::mat3_cast(Rotation));
    glm::dvec3 localOrigin = invRot * (glm::dvec3(ray.origin()) - glm::dvec3(Position));
    glm::dvec3 localDir = invRot * glm::dvec3(ray.direction());

    glm::dvec3 minBounds = -glm::dvec3(_halfSize);
    glm::dvec3 maxBounds = glm::dvec3(_halfSize);

    double tmin = ray_t.min;
    double tmax = ray_t.max;
    int enterAxis = -1;
    double enterSign = 0.0;
    int exitAxis = -1;
    double exitSign = 0.0;

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
                enterAxis = i;
                enterSign = (invD < 0.0) ? 1.0 : -1.0;
            }
            if (t1 < tmax)
            {
                tmax = t1;
                exitAxis = i;
                exitSign = (invD < 0.0) ? -1.0 : 1.0;
            }

            if (tmax <= tmin)
                return {};
        }
    }

    double hitT = tmin;
    int hitAxis = enterAxis;
    double hitSign = enterSign;

    if (hitT <= ray_t.min)
    {
        hitT = tmax;
        hitAxis = exitAxis;
        hitSign = exitSign;
    }

    if (hitAxis == -1 || hitT <= ray_t.min || hitT >= ray_t.max)
    {
        return {};
    }

    HitResult hit;
    hit.t = hitT;
    glm::dvec3 p_precise = glm::dvec3(ray.origin()) + hitT * glm::dvec3(ray.direction());
    hit.p = glm::vec3(p_precise);

    glm::dvec3 localNormal(0.0);
    localNormal[hitAxis] = hitSign;

    const glm::vec3 outwardNormal = glm::normalize(glm::mat3_cast(Rotation) * glm::vec3(localNormal));
    hit.SetObjectHit(this, ray, outwardNormal);

    return hit;
}
