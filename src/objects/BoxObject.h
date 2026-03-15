// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "ObjectBase.h"


class BoxObject : public ObjectBase
{
public:
    ~BoxObject() override = default;
    BoxObject(const glm::vec3& center, const glm::vec3& size);
    BoxObject(const glm::vec3& center, const glm::vec3& size, const glm::vec3& rotationEuler);

    HitResult HitInRayInterval(Ray ray, Interval ray_t) const override;

private:
    glm::vec3 _halfSize;
    glm::mat3 _invRotation;
};