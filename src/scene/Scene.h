// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <memory>
#include <vector>

class Interval;
class Ray;
class ObjectBase;
struct HitResult;

class Scene
{
public:
    Scene() = default;

    void add(std::shared_ptr<ObjectBase> object);

    HitResult HitAny(Ray ray, Interval ray_t) const;

protected:
    std::vector<std::shared_ptr<ObjectBase>> _objects;
};