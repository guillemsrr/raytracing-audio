// Copyright (c) Guillem Serra. All Rights Reserved.

#include "AudioTracer.h"

#include "graphics/materials/Material.h"
#include "objects/ObjectBase.h"

AudioTracer::AudioTracer(Camera* camera) : RaytracerBase(camera)
{
}

void AudioTracer::SetAudioEmitter(const AudioEmitter* emitter)
{
    _audioEmitter = emitter;
}

color AudioTracer::TraceRay(const Ray& ray) const
{
    //TODO
    return {0, 0, 0, 1.0f};
}
