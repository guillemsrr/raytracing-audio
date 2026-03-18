// Copyright (c) Guillem Serra. All Rights Reserved.

#include "WhittedRaytracer.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

#include "graphics/materials/Material.h"
#include "objects/ObjectBase.h"
#include "base/HitResult.h"
#include "base/Ray.h"
#include "raytracing/base/Interval.h"

WhittedRaytracer::WhittedRaytracer(Camera* camera) : RaytracerBase(camera)
{
}

color WhittedRaytracer::TraceRay(const Ray& ray) const
{
    const HitResult hit = _scene->HitAny(ray, Interval(_shadowBias, FLT_MAX));
    if (!hit.HasHit())
    {
        return GetSkyColor(ray.direction());
    }

    return ShadeSurface(hit, ray, 0);
}

color WhittedRaytracer::ShadeSurface(const HitResult& hit, const Ray& ray, int depth) const
{
    const Material& material = *hit.ObjectHit->GetMaterial();
    const DirectionalLight& sunLight = _scene->GetSunLight();
    const glm::vec3 lightDirection = glm::normalize(sunLight.Direction);
    
    // Ambient
    glm::vec3 lighting = _scene->GetAmbientLight() * glm::vec3(material.Albedo) * material.AO;

    // Direct Diffuse & Specular
    if (!IsOccluded(hit.p + hit.normal * _shadowBias, lightDirection))
    {
        const float diffuseFactor = std::max(glm::dot(hit.normal, lightDirection), 0.0f);
        if (diffuseFactor > 0.0f)
        {
            const glm::vec3 diffuse = glm::vec3(material.Albedo) * sunLight.Color * sunLight.Intensity * diffuseFactor;
            
            const glm::vec3 viewDirection = glm::normalize(-ray.direction());
            const glm::vec3 reflectedLight = glm::reflect(-lightDirection, hit.normal);
            const float shininess = glm::mix(12.0f, 96.0f, 1.0f - glm::clamp(material.Roughness, 0.0f, 1.0f));
            const float specularFactor = std::pow(std::max(glm::dot(viewDirection, reflectedLight), 0.0f), shininess);
            const glm::vec3 specular = sunLight.Color * sunLight.Intensity * specularFactor * material.Specular;
            
            lighting += diffuse + specular;
        }
    }

    // Emission
    lighting += glm::vec3(material.Albedo) * material.Emission;

    color finalColor(lighting, 1.0f);

    // Recursive reflections (Whitted-style)
    if (depth < _maxBounces)
    {
        const float reflectivity = glm::clamp(material.Reflectivity + material.Metallic, 0.0f, 0.98f);
        if (reflectivity > 0.01f)
        {
            const glm::vec3 reflectionDir = glm::reflect(ray.direction(), hit.normal);
            const Ray reflectionRay(hit.p + hit.normal * _shadowBias, reflectionDir);
            const HitResult reflectHit = _scene->HitAny(reflectionRay, Interval(_shadowBias, FLT_MAX));
            
            color reflectedColor;
            if (reflectHit.HasHit())
                reflectedColor = ShadeSurface(reflectHit, reflectionRay, depth + 1);
            else
                reflectedColor = GetSkyColor(reflectionDir);

            const glm::vec3 tint = glm::mix(glm::vec3(1.0f), glm::vec3(material.Albedo), material.Metallic);
            finalColor = glm::mix(finalColor, reflectedColor * color(tint, 1.0f), reflectivity);
        }
    }

    return finalColor;
}

color WhittedRaytracer::GetSkyColor(const glm::vec3& rayDirection) const
{
    const float blend = 0.5f * (rayDirection.y + 1.0f);
    const color horizon(_scene->GetSkyHorizonColor(), 1.0f);
    const color zenith(_scene->GetSkyZenithColor(), 1.0f);
    return glm::mix(horizon, zenith, blend);
}

bool WhittedRaytracer::IsOccluded(const glm::vec3& origin, const glm::vec3& direction) const
{
    const Ray shadowRay(origin, direction);
    return _scene->HitAny(shadowRay, Interval(_shadowBias, FLT_MAX)).HasHit();
}
