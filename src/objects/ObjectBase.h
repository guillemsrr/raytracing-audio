// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <future>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

class Material;
class Ray;
class Interval;
struct HitResult;

class ObjectBase
{
protected:
    ~ObjectBase() = default;

public:
    explicit ObjectBase(glm::vec3 center);

    //TODO: find why it can't compile if definition in .cpp
    void SetMaterial(const std::shared_ptr<Material>& material)
    {
        _material = material;
    }

    const Material* GetMaterial() const;

    virtual HitResult HitInRayInterval(Ray ray, Interval ray_t) = 0;

protected:
    glm::vec3 _center;
    glm::mat3 _rotation;

    std::shared_ptr<Material> _material;
};