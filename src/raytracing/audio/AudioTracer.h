// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "../base/RaytracerBase.h"
#include "audio/AudioEmitter.h"

class AudioListener;

class AudioTracer : public RaytracerBase
{
public:
    explicit AudioTracer(Camera* camera);

    void SetAudioEmitter(const AudioEmitter* emitter);

protected:
    color TraceRay(const Ray& ray) const override;
    //TODO: sound doesn't do color
    void TracePath(AudioEmitter* emitter, vec3 listenerPosition);

private:
    const AudioEmitter* _audioEmitter = nullptr;
    int _maxBounces = 10;
    int _maxSoundDistance = 100000;
    
    float _binDuration = 0.001f;         // each bin covers 1 ms of arrival time                                                                                                                                                                                                                                    
    float _maxTime     = 2.0f;           // ignore anything arriving after 2 s  
    
    //For impulse responses, analogue of _colorBuffer
    std::vector<glm::vec3> _histogram;   // one vec3 (3 bands) of energy per bin
    
    AudioListener* listener;
};
