// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "Material.h"

#include <glm/vec3.hpp>

namespace MaterialPresets
{
    inline Material Emissive(const glm::vec3& color, float intensity = 1.0f)
    {
        Material mat;
        mat.Albedo = glm::vec4(color, 1.0f);
        mat.Emission = intensity;
        mat.Transparency = 0.0f;
        return mat;
    }

    inline Material Metallic(const glm::vec3& color = glm::vec3(1.f))
    {
        Material mat;
        mat.Albedo = glm::vec4(color, 1.0f);
        mat.Metallic = 1.0f;
        mat.Roughness = 0.3f;
        return mat;
    }

    inline Material Concrete()
    {
        Material mat;
        mat.Albedo = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        mat.Metallic = 0.0f;
        mat.Roughness = 0.9f;
        mat.AO = 1.0f;
        return mat;
    }

    inline Material Glass(float ior = 1.5f)
    {
        Material mat;
        mat.Albedo = glm::vec4(1.0f);
        mat.Transparency = 1.0f;
        mat.IOR = ior;
        mat.RefractionIndex = ior;
        mat.Fresnel = 1.0f;
        return mat;
    }
}