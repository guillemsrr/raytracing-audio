// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "Scene.h"


class RotatingScene : public Scene
{
public:
    RotatingScene();

    void Update(float deltaTime) override;

private:
    float _time = 0.0f;
};
