// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <vector>

#include "graphics/Camera.h"
#include "graphics/RendererBase.h"
#include "graphics/Shader.h"
#include "graphics/shapes/CubeRenderer.h"
#include "raytracing/Ray.h"
#include "renderShapes/ScreenQuadShape.h"
#include "scene/Scene.h"

#include <SDL3/SDL_video.h>

using color = glm::vec3;

class Renderer : public RendererBase
{
public:
    Renderer(SDL_Window* window, Camera* const camera);
    void SetScene(Scene& scene);

    void RenderRaytracing();
    void RenderDebug();

private:
    ScreenQuadShape _screenQuadRenderer;
    CubeRenderer _cubeRenderer;

    GLuint _raytracedTexture = 0;

    SDL_Window* _window;

    int _screenWidth = 0;
    int _screenHeight = 0;

    Shader _raytracingShader;
    Shader _debugShader;

    const Scene* _scene = nullptr;
    int _pixelSize = 1;
    int _bounces = 2;

    std::vector<uint8_t> _raytracingTextureVector;

    vec3 _lightDir = glm::normalize(glm::vec3(-1.f, -1.f, -1.f));

    void GenerateRGBImage();
    void RayTraceScreen();
    color RayTracePixelColor(glm::vec3 pixel_pos);

    void write_color(const color& pixel_color, int i, int j, int pixelSize);
    void write_color(const color& pixel_color, int index);

    const uint8_t bytes_per_pixel = 3;
};