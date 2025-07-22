// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

class Ray;
class Interval;
struct HitResult;

class ObjectBase
{
public:
    glm::vec4 Albedo = glm::vec4(1.0f);
    float Metallic;
    float Roughness;
    float RefractionIndex;
    float AO;
    float Emission;
    float Transparency;
    float Fresnel;
    float IOR;
    float Dissolve;

    virtual HitResult HitInRayInterval(Ray ray, Interval ray_t) = 0;
};