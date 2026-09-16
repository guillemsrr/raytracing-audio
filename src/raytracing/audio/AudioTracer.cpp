// Copyright (c) Guillem Serra. All Rights Reserved.

#include "AudioTracer.h"
#include "AudioListener.h"

#include "graphics/materials/Material.h"
#include "objects/ObjectBase.h"

#include "raytracing/base/HitResult.h"
#include "raytracing/base/Interval.h"
#include "raytracing/base/Ray.h"

#include "utils/Utils.h"

AudioTracer::AudioTracer(Camera* camera) : RaytracerBase(camera)
{
    _histogram.assign(static_cast<size_t>(_maxTime / _binDuration), glm::vec3(0.0f)); //→ 2000 bins.
}

void AudioTracer::SetAudioEmitter(const AudioEmitter* emitter)
{
    _audioEmitter = emitter;
}

void AudioTracer::TracePath(AudioEmitter* emitter, AudioListener* listener)
{
    this->listener = listener;
    Ray ray(emitter->Position, Utils::RandomUnitVector());

    std::array<color> _histogram;
    color rayResult = TraceRay(ray);
    _histogram[0] = rayResult;
}

color AudioTracer::TraceRay(const Ray& ray) const
{
    glm::vec3 throughput(1.0f);
    // per frequency band                                                                                                                                                                                                                                                     
    float distance = 0.0f;
    for (int bounce = 0; bounce < _maxBounces; ++bounce)
    {
        const HitResult hit = _scene->HitAny(ray, Interval(_shadowBias, _maxSoundDistance));
        if (!hit.HasHit())
        {
            break;
        }

        distance += hit.totalDistance;

        const glm::vec3 listenerDirection = listener->Position - hit.p;
        const float listenerDistance = glm::length(listenerDirection);
        if (!IsOccluded(hit.p + hit.normal * _shadowBias,
                        listenerDirection / listenerDistance,
                        Interval(_shadowBias, distance - _shadowBias)))
        {
            //TODO: global
            const float soundSpeed = 343;
            const float arrivalTime = distance / soundSpeed;

            const int bin = _histogram.at(arrivalTime);
            if (bin > _histogram.size()))
            {
                _histogram[bin] += throughput;
            }
        }

        throughput *= (glm::vec3(1) - hit.ObjectHit->GetMaterial()->Absorption.r);
        //TODO: what is compMax
        if (std::max({throughput.x, throughput.y, throughput.z}) < 0.01f)
            break;

        //dIFFUSE only. what does this have to be for diffuse?
        const glm::vec3 bounceDir = glm::normalize(hit.normal + Utils::RandomUnitVector());

        //Later refinement: Material::Scattering is the acoustic field that means "fraction of energy scattered diffusely vs. reflected like a mirror" — so if (Utils::RandomFloat() < material.Scattering) diffuse else reflect, mirroring the Reflectivity branch. Leave it as a TODO.
        // steal from PathRaytracer — which branch?                                                                                                                                                                                                          
        ray = Ray(hit.p + hit.normal * _shadowBias, bounceDir);
    }
}
