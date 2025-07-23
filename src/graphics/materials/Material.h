// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <glm/vec4.hpp>

struct Material
{
    glm::vec4 Albedo = glm::vec4(1.0f);
    float Metallic = 0.0f;
    float Roughness = 1.f;
    
    float Specular;
    float RefractionIndex;
    float AO;
    float Emission;
    float Transparency;
    float Fresnel;
    float IOR;
    float Dissolve;
};