// Copyright (c) Guillem Serra. All Rights Reserved.

#include "WhittedTracer.h"

#include <algorithm>
#include <cmath>

#include "graphics/materials/Material.h"
#include "objects/ObjectBase.h"
#include "raytracing/base/HitResult.h"
#include "raytracing/base/Ray.h"

namespace Light
{
    WhittedTracer::WhittedTracer(Camera* camera) : RaytracerBase(camera)
    {
    }

    color WhittedTracer::TraceRay(const Ray& ray) const
    {
        const HitResult hit = _intersector.ClosestHit(ray);
        if (!hit.HasHit())
        {
            return GetSkyColor(ray.direction());
        }

        return ShadeSurface(hit, ray, 0);
    }

    color WhittedTracer::ShadeSurface(const HitResult& hit, const Ray& ray, int depth) const
    {
        const Material& material = *hit.ObjectHit->GetMaterial();
        const DirectionalLight& sunLight = _scene->GetSunLight();
        const glm::vec3 lightDirection = glm::normalize(sunLight.Direction);

        // Ambient
        glm::vec3 lighting = _scene->GetAmbientLight() * glm::vec3(material.Albedo) * material.AO;

        // Direct Diffuse & Specular
        if (!_intersector.IsOccluded(_intersector.OffsetFromSurface(hit), lightDirection))
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
                const Ray reflectionRay(_intersector.OffsetFromSurface(hit), reflectionDir);
                const HitResult reflectHit = _intersector.ClosestHit(reflectionRay);

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

    color WhittedTracer::GetSkyColor(const glm::vec3& rayDirection) const
    {
        const float blend = 0.5f * (rayDirection.y + 1.0f);
        const color horizon(_scene->GetSkyHorizonColor(), 1.0f);
        const color zenith(_scene->GetSkyZenithColor(), 1.0f);
        return glm::mix(horizon, zenith, blend);
    }
}
