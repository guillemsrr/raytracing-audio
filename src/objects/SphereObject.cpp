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
    glm::vec3 oc = _center - ray.origin();
    auto a = Utils::LengthSquared(ray.direction());
    auto h = dot(ray.direction(), oc);
    auto c = Utils::LengthSquared(oc) - _radius * _radius;

    auto discriminant = h * h - a * c;
    if (discriminant < 0)
    {
        return {};
    }

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root))
    {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root))
            return {};
    }

    HitResult hitResult = HitResult();

    hitResult.t = root;
    hitResult.p = ray.at(hitResult.t);
    const glm::vec3 outwardNormal = glm::normalize((hitResult.p - _center) / _radius);
    hitResult.SetObjectHit(this, ray, outwardNormal);

    return hitResult;
}
