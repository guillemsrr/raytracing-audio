// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "graphics/CubeRenderer.h"
#include "graphics/RendererBase.h"

class Renderer : public RendererBase
{
public:
    Renderer(Camera* const camera);

    void RenderPlane();

private:
    CubeRenderer _cubeRenderer;
    unsigned int _generalShader = 0;
};