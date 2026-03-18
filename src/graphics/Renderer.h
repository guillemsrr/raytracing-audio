// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <cstdint>
#include <glm/vec4.hpp>
#include "graphics/Color.h"
#include "graphics/Camera.h"
#include "graphics/RendererBase.h"
#include "graphics/Shader.h"
#include "graphics/shapes/CubeRenderer.h"
#include "renderShapes/ScreenQuadShape.h"
#include "../raytracing/base/IRaytracer.h"

#include <SDL3/SDL_video.h>


class Renderer : public RendererBase
{
public:
    Renderer(SDL_Window* window, Camera* const camera);
    ~Renderer() override;

    bool SyncWindowSize();
    void Present(const uint32_t* pixels, int width, int height);
    void UpdateAndRender(IRaytracer* raytracer);
    void RenderDebug();

    void RenderBackground() {}

private:
    ScreenQuadShape _screenQuadRenderer;
    CubeRenderer _cubeRenderer;

    GLuint _raytracedTexture = 0;

    SDL_Window* _window;

    int _screenWidth = 0;
    int _screenHeight = 0;

    Shader _raytracingShader;
    Shader _debugShader;

    void OnscreenResize();
};