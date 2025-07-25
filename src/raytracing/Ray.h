// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <glm/vec3.hpp>

using vec3 = glm::vec3;

class Ray
{
public:
    Ray(const vec3& origin, const vec3& direction);

    const vec3& origin() const
    {
        return orig;
    }

    const vec3& direction() const
    {
        return dir;
    }

    vec3 at(float t) const
    {
        return orig + t * dir;
    }

private:
    vec3 orig;
    vec3 dir;
};