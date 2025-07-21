// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <vector>

#include "graphics/Camera.h"
#include "graphics/RendererBase.h"
#include "graphics/Shader.h"

#include "raytracing/ScreenQuadShape.h"

#include <SDL3/SDL_video.h>

class Renderer : public RendererBase
{
public:
    Renderer(SDL_Window* window, Camera* const camera);

    void RenderRaytracing();

private:
    ScreenQuadShape _screenQuadRenderer;

    GLuint _raytracedTexture = 0;

    SDL_Window* _window;

    int _lastScreenWidth = 0;
    int _lastScreenHeight = 0;

    Shader _generalShader;
    Shader _raytracingShader;

    void GenerateRGBImage(int screenWidth, int screenHeight, std::vector<unsigned char>& image);
};