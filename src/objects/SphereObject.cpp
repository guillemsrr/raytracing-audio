#include "SphereObject.h"

#include "raytracing/Ray.h"
#include "raytracing/Interval.h"
#include "raytracing/HitResult.h"

#include <glm/geometric.hpp>

#include "utils/Utils.h"

SphereObject::SphereObject(const glm::vec3& center, double radius) : ObjectBase(center), _radius(std::fmax(0.0, radius))
{
}

HitResult SphereObject::HitInRayInterval(Ray ray, Interval ray_t) const
{
    glm::dvec3 oc = glm::dvec3(_center) - glm::dvec3(ray.origin());
    glm::dvec3 rayDirection = glm::dvec3(ray.direction());
    double a = glm::dot(rayDirection, rayDirection);
    double h = glm::dot(rayDirection, oc);
    double c = glm::dot(oc, oc) - _radius * _radius;

    double discriminant = h * h - a * c;
    if (discriminant < 0)
    {
        return {};
    }

    double sqrtd = std::sqrt(discriminant);

    double root = (h - sqrtd) / a;
    if (root <= ray_t.min || root >= ray_t.max)
    {
        root = (h + sqrtd) / a;
        if (root <= ray_t.min || root >= ray_t.max)
            return {};
    }

    HitResult hitResult = HitResult();

    hitResult.t = root;
    glm::dvec3 p_precise = glm::dvec3(ray.origin()) + root * rayDirection;
    hitResult.p = glm::vec3(p_precise);

    const glm::vec3 outwardNormal = glm::vec3((p_precise - glm::dvec3(_center)) / _radius);
    hitResult.SetObjectHit(this, ray, outwardNormal);

    return hitResult;
}
