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
    
    std::vector<std::shared_ptr<ObjectBase>> objects;

    void clear()
    {
        objects.clear();
    }

    void add(std::shared_ptr<ObjectBase> object)
    {
        objects.push_back(object);
    }

    HitResult HitAny(Ray ray, Interval ray_t) const;
};