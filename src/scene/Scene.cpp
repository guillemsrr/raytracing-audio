#include "Scene.h"

#include "raytracing/HitResult.h"
#include "raytracing/Interval.h"
#include "raytracing//Ray.h"
#include "objects/ObjectBase.h"

void Scene::add(std::shared_ptr<ObjectBase> object)
{
    _objects.push_back(object);
}

HitResult Scene::HitAny(Ray ray, Interval ray_t) const
{
    auto temp_rec = HitResult();

    auto closest_so_far = ray_t.max;

    for (const auto& object : _objects)
    {
        HitResult hit = object->HitInRayInterval(ray, Interval(ray_t.min, closest_so_far));
        if (hit.HasHit())
        {
            closest_so_far = temp_rec.t;
            temp_rec = hit;
        }
    }

    return temp_rec;
}