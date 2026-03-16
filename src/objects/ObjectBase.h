// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <future>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

struct Material;
class Ray;
class Interval;
struct HitResult;

class ObjectBase
{
public:
    virtual ~ObjectBase() = default;
    explicit ObjectBase(glm::vec3 center);

    void SetMaterial(const std::shared_ptr<Material>& material)
    {
        _material = material;
    }

    const Material* GetMaterial() const;
    const glm::vec3& GetCenter() const;

    virtual HitResult HitInRayInterval(Ray ray, Interval ray_t) const = 0;

protected:
    glm::vec3 _center;
    glm::mat3 _rotation;

    std::shared_ptr<Material> _material;
};