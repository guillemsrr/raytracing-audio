// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "Scene.h"


class MovingObjectsScene : public Scene
{
public:
    MovingObjectsScene();

    void Update(float deltaTime) override;

private:
    float _time = 0.0f;
};
