#include "SphereObject.h"

#include "../raytracing/Ray.h"
#include "../raytracing/Interval.h"
#include "../raytracing/HitResult.h"

#include <glm/geometric.hpp>

#include "utils/Utils.h"

SphereObject::SphereObject(const glm::vec3& center, double radius) : center(center), radius(std::fmax(0, radius))
{
}

HitResult SphereObject::HitInRayInterval(Ray ray, Interval ray_t)
{
    glm::vec3 oc = center - ray.origin();
    auto a = Utils::LengthSquared(ray.direction());
    auto h = dot(ray.direction(), oc);
    auto c = Utils::LengthSquared(oc) - radius * radius;

    auto discriminant = h * h - a * c;
    if (discriminant < 0)
        return {};

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root))
    {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root))
            return {};
    }

    HitResult hitResult = HitResult();

    hitResult.t = root;
    hitResult.p = ray.at(hitResult.t);
    hitResult.normal = (hitResult.p - center) / radius;
    glm::vec3 outward_normal = (hitResult.p - center) / radius;
    hitResult.SetObjectHit(this, ray, outward_normal);

    return hitResult;
}