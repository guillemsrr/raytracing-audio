// Copyright (c) Guillem Serra. All Rights Reserved.

#include "PathRaytracer.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

#include "graphics/materials/Material.h"
#include "objects/ObjectBase.h"
#include "base/HitResult.h"
#include "base/Ray.h"
#include "raytracing/base/Interval.h"

#include "utils/Utils.h"

PathRaytracer::PathRaytracer(Camera* camera) : RaytracerBase(camera)
{
}

color PathRaytracer::TraceRay(const Ray& ray) const
{
    color radiance(0.0f);
    color throughput(1.0f);
    Ray currentRay = ray;

    for (int bounce = 0; bounce < _maxBounces; ++bounce)
    {
        const HitResult hit = _scene->HitAny(currentRay, Interval(_shadowBias, FLT_MAX));
        if (!hit.HasHit())
        {
            radiance += throughput * GetSkyColor(currentRay.direction());
            break;
        }

        const Material& material = *hit.ObjectHit->GetMaterial();
        const float reflectivity = glm::clamp(material.Reflectivity + material.Metallic, 0.0f, 0.98f);

        // Direct emission
        radiance += throughput * color(glm::vec3(material.Albedo) * material.Emission, 1.0f);

        // Direct lighting from sun (Next Event Estimation)
        radiance += throughput * ShadeSurface(hit, currentRay);

        // Stochastic bounce
        glm::vec3 bounceDirection;
        const float roughness = glm::clamp(material.Roughness, 0.0f, 1.0f);

        if (Utils::RandomFloat() < reflectivity)
        {
            // Specular reflection
            const glm::vec3 reflection = glm::reflect(currentRay.direction(), hit.normal);
            // Apply roughness perturbation to reflection
            bounceDirection = glm::normalize(reflection + roughness * Utils::RandomInUnitSphere());

            // Tint throughput by albedo for metallic surfaces
            const glm::vec3 tint = glm::mix(glm::vec3(1.0f), glm::vec3(material.Albedo), material.Metallic);
            throughput *= color(tint, 1.0f);
        }
        else
        {
            // Diffuse reflection (cosine weighted approximation)
            bounceDirection = glm::normalize(hit.normal + Utils::RandomUnitVector());
            throughput *= color(glm::vec3(material.Albedo), 1.0f);
        }

        // Russian Roulette or low throughput termination
        const float maxThroughput = std::max({throughput.r, throughput.g, throughput.b});
        if (maxThroughput <= 0.01f)
        {
            break;
        }

        // Offset origin to avoid self-intersection
        currentRay = Ray(hit.p + hit.normal * _shadowBias, bounceDirection);
    }

    radiance.a = 1.0f;
    return radiance;
}

color PathRaytracer::ShadeSurface(const HitResult& hit, const Ray& ray) const
{
    const Material& material = *hit.ObjectHit->GetMaterial();
    const DirectionalLight& sunLight = _scene->GetSunLight();
    const glm::vec3 lightDirection = glm::normalize(sunLight.Direction); // Direction points towards the light source
    
    // Use ambient light only as a small fill light to avoid pitch black in the first frame.
    // In a full path tracer, the sky handles this after the first bounce.
    glm::vec3 lighting = _scene->GetAmbientLight() * glm::vec3(material.Albedo) * material.AO * 0.1f;

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

    return color(lighting, 1.0f);
}

color PathRaytracer::GetSkyColor(const glm::vec3& rayDirection) const
{
    const float blend = 0.5f * (rayDirection.y + 1.0f);
    const color horizon(_scene->GetSkyHorizonColor(), 1.0f);
    const color zenith(_scene->GetSkyZenithColor(), 1.0f);
    return glm::mix(horizon, zenith, blend);
}

bool PathRaytracer::IsOccluded(const glm::vec3& origin, const glm::vec3& direction) const
{
    const Ray shadowRay(origin, direction);
    return _scene->HitAny(shadowRay, Interval(_shadowBias, FLT_MAX)).HasHit();
}