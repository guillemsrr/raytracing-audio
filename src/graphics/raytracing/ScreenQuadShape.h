// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "graphics/shapes/ShapeRenderer.h"

class ScreenQuadShape: public ShapeRenderer
{
public:
    void Init() override;
    void Draw() override;
};