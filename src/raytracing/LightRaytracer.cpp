// Copyright (c) Guillem Serra. All Rights Reserved.

#include "LightRaytracer.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

#include "graphics/materials/Material.h"
#include "objects/ObjectBase.h"
#include "raytracing/HitResult.h"
#include "raytracing/Interval.h"

LightRaytracer::LightRaytracer(Camera* camera) : RaytracerBase(camera)
{
}

color LightRaytracer::TraceRay(const Ray& ray) const
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
        const float reflectivity = glm::clamp(material.Reflectivity + material.Metallic * 0.15f, 0.0f, 0.98f);

        radiance += throughput * color(glm::vec3(material.Albedo) * material.Emission, 1.0f);
        radiance += throughput * ShadeSurface(hit, currentRay) * (1.0f - reflectivity);

        if (bounce + 1 >= _maxBounces || reflectivity <= 0.01f)
        {
            break;
        }

        glm::vec3 reflectionDirection = glm::reflect(currentRay.direction(), hit.normal);
        const float roughnessBlend = glm::clamp(material.Roughness * 0.15f, 0.0f, 0.15f);
        reflectionDirection = glm::normalize(glm::mix(reflectionDirection, hit.normal, roughnessBlend));

        const glm::vec3 tint = glm::mix(glm::vec3(1.0f), glm::vec3(material.Albedo), material.Metallic);
        throughput *= color(tint * reflectivity, 1.0f);

        const float maxThroughput = std::max({throughput.r, throughput.g, throughput.b});
        if (maxThroughput <= 0.01f)
        {
            break;
        }

        currentRay = Ray(hit.p + hit.normal * _shadowBias, reflectionDirection);
    }

    radiance.a = 1.0f;
    return radiance;
}

color LightRaytracer::ShadeSurface(const HitResult& hit, const Ray& ray) const
{
    const Material& material = *hit.ObjectHit->GetMaterial();
    const DirectionalLight& sunLight = _scene->GetSunLight();
    const glm::vec3 lightDirection = glm::normalize(sunLight.Direction); // Direction points towards the light source
    glm::vec3 lighting = _scene->GetAmbientLight() * glm::vec3(material.Albedo) * material.AO;

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

color LightRaytracer::GetSkyColor(const glm::vec3& rayDirection) const
{
    const float blend = 0.5f * (rayDirection.y + 1.0f);
    const color horizon(_scene->GetSkyHorizonColor(), 1.0f);
    const color zenith(_scene->GetSkyZenithColor(), 1.0f);
    return glm::mix(horizon, zenith, blend);
}

bool LightRaytracer::IsOccluded(const glm::vec3& origin, const glm::vec3& direction) const
{
    const Ray shadowRay(origin, direction);
    return _scene->HitAny(shadowRay, Interval(_shadowBias, FLT_MAX)).HasHit();
}