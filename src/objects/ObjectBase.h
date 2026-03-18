// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <future>
#include <glm/fwd.hpp>

#include "core/Actor.h"

struct Material;
class Ray;
class Interval;
struct HitResult;

class ObjectBase : public Actor
{
public:
    virtual ~ObjectBase() = default;
    explicit ObjectBase(glm::vec3 center);

    void SetMaterial(const std::shared_ptr<Material>& material)
    {
        _material = material;
    }

    const Material* GetMaterial() const;

    virtual HitResult HitInRayInterval(Ray ray, Interval ray_t) const = 0;

protected:
    std::shared_ptr<Material> _material;
};
