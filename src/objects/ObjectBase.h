// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <future>

class Material;
class Ray;
class Interval;
struct HitResult;

class ObjectBase
{
public:
    ObjectBase();

    //TODO: find why it can't compile if definition in .cpp
    void SetMaterial(const std::shared_ptr<Material>& material)
    {
        _material = material;
    }

    const Material* GetMaterial() const;

    virtual HitResult HitInRayInterval(Ray ray, Interval ray_t) = 0;

protected:
    std::shared_ptr<Material> _material;
};