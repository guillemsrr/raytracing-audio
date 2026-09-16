// Copyright (c) Guillem Serra. All Rights Reserved.

#include "Propagator.h"

#include <algorithm>

#include "graphics/materials/Material.h"
#include "objects/ObjectBase.h"
#include "raytracing/base/HitResult.h"
#include "raytracing/base/Intersector.h"
#include "raytracing/base/Ray.h"
#include "utils/Utils.h"

namespace Audio
{
    void Propagator::PropagateDirect(const Intersector& intersector,
                                     const glm::vec3& origin,
                                     const glm::vec3& target,
                                     const ArrivalCallback& onArrival) const
    {
        const glm::vec3 toTarget = target - origin;
        const float targetDistance = glm::length(toTarget);
        if (targetDistance <= intersector.GetBias())
            return;

        if (!intersector.IsOccluded(origin, toTarget / targetDistance, targetDistance))
            onArrival(targetDistance, glm::vec3(1.0f));
    }

    void Propagator::Propagate(const Intersector& intersector,
                               const glm::vec3& origin,
                               const glm::vec3& direction,
                               const glm::vec3& target,
                               const ArrivalCallback& onArrival) const
    {
        glm::vec3 throughput(1.0f); // per frequency band (low/mid/high)
        float pathLength = 0.0f;
        Ray ray(origin, direction);

        for (int bounce = 0; bounce < MaxBounces; ++bounce)
        {
            const HitResult hit = intersector.ClosestHit(ray, MaxRayDistance);
            if (!hit.HasHit())
            {
                break; // escaped the scene: no sky for sound, nothing comes back
            }

            pathLength += hit.distance;

            // Next event estimation toward the target
            const glm::vec3 surfacePoint = intersector.OffsetFromSurface(hit);
            const glm::vec3 targetDirection = target - surfacePoint;
            const float targetDistance = glm::length(targetDirection);
            if (targetDistance > intersector.GetBias() && !intersector.IsOccluded(
                    surfacePoint,
                    targetDirection / targetDistance,
                    targetDistance))
                // The sound travelled the whole path so far, plus the last leg to the target.
                // TODO: attenuate the last leg with distance (1/d^2) once the histogram is audible.
                onArrival(pathLength + targetDistance, throughput);

            // Surface absorbs part of each band; the rest keeps bouncing (sound analogue of "*= Albedo").
            const Material& material = *hit.ObjectHit->GetMaterial();
            throughput *= glm::vec3(1.0f) - material.Absorption;

            if (std::max({throughput.x, throughput.y, throughput.z}) < 0.01f)
            {
                break;
            }

            const glm::vec3 bounceDirection = glm::normalize(hit.normal + Utils::RandomUnitVector());
            ray = Ray(surfacePoint, bounceDirection);
        }
    }
}
