// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "graphics/Camera.h"
#include "graphics/RendererBase.h"
#include "graphics/Shader.h"
#include "graphics/shapes/CubeRenderer.h"
#include "raytracing/Ray.h"
#include "renderShapes/ScreenQuadShape.h"
#include "scene/Scene.h"

#include <SDL3/SDL_video.h>

using color = glm::vec4;

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
    int _bounces = 10;

    size_t _raytracingTextureBufferSize = 0;
    uint32_t* _raytracingTextureBuffer = nullptr;
    glm::vec4* _accumulation = nullptr;
    uint32_t _frameIndex = 1;

    vec3 _lightDir = glm::normalize(glm::vec3(-1.f, -1.f, -1.f));

    const float HIT_EPSILON = 0.01f;

    std::vector<int> _pixelScreenHorizontalIterator, _pixelScreenVerticalIterator;

    void RayTraceScreenPixel(glm::vec3 pixelPosition, int index);
    void OnscreenResize();
    void GenerateRGBImage();
    void RayTraceScreen();
    void RayTraceScreen2();
    color RayTracePixelColor(Ray ray);
    color RayTracePixelColor2(glm::vec2 pixelPosition);

    void WritePixelColorColor(const color& pixelColor, int i, int j);
    void ResetFrameIndex();
};