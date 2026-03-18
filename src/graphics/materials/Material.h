// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "graphics/Color.h"

struct Material
{
    color Albedo = Colors::White;
    float Metallic = 0.0f;
    float Roughness = 0.5f;
    
    float Specular = 0.35f;
    float Reflectivity = 0.0f;
    float RefractionIndex = 1.0f;
    float AO = 1.0f;
    float Emission = 0.0f;
    float Transparency = 0.0f;
    float Fresnel = 0.04f;
    float IOR = 1.0f;
    float Dissolve = 1.0f;

    glm::vec3 Absorption = glm::vec3(0.15f);
    float Scattering = 0.0f;
    float AcousticReflectivity = 0.0f;
};