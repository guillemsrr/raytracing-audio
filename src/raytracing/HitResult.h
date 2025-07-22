// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

class ObjectBase;
class Ray;

struct HitResult
{
    HitResult() = default;

    glm::vec3 p;
    glm::vec3 normal;
    glm::vec3 direction;

    double t;
    bool front_face;

    ObjectBase* ObjectHit = nullptr;

    void SetObjectHit(ObjectBase* object, const Ray& r, const glm::vec3& outward_normal);
    bool HasHit() const;

private:
    bool _hasHit = false;
};