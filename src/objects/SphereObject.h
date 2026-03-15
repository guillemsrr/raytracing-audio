// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "ObjectBase.h"

class SphereObject : public ObjectBase
{
public:
    ~SphereObject() = default;
    SphereObject(const glm::vec3& center, double radius);
    HitResult HitInRayInterval(Ray ray, Interval ray_t) const override;

private:
    float _radius;
};
