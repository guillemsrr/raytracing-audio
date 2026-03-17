#include "Scene.h"

#include "raytracing/HitResult.h"
#include "raytracing/Interval.h"
#include "raytracing//Ray.h"
#include "objects/ObjectBase.h"


void Scene::Add(std::shared_ptr<ObjectBase> object)
{
    _objects.push_back(object);
}

const std::vector<std::shared_ptr<ObjectBase>>& Scene::GetObjects() const
{
    return _objects;
}

HitResult Scene::HitAny(Ray ray, Interval ray_t) const
{
    auto temp_rec = HitResult();

    auto closestSoFar = ray_t.max;

    for (const auto& object : _objects)
    {
        HitResult hit = object->HitInRayInterval(ray, Interval(ray_t.min, closestSoFar));
        if (hit.HasHit())
        {
            closestSoFar = hit.t;
            temp_rec = hit;
        }
    }

    return temp_rec;
}
