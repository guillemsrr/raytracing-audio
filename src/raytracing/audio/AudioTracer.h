// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "../base/RaytracerBase.h"
#include "CandidateHit.h"

#include "audio/AudioEmitter.h"

class AudioTracer : public RaytracerBase
{
public:
    explicit AudioTracer(Camera* camera);

    void SetAudioEmitter(const AudioEmitter* emitter);

protected:
    color TraceRay(const Ray& ray) const override;

private:
    const AudioEmitter* _audioEmitter = nullptr;
};
