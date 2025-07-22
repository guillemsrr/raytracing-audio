// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <vector>

#include "graphics/Camera.h"
#include "graphics/Camera.h"
#include "graphics/Camera.h"
#include "graphics/RendererBase.h"
#include "graphics/Shader.h"
#include "graphics/shapes/CubeRenderer.h"

#include "raytracing/Ray.h"
#include "raytracing/ScreenQuadShape.h"

#include <SDL3/SDL_video.h>

using color = glm::vec3;

class Renderer : public RendererBase
{
public:
    Renderer(SDL_Window* window, Camera* const camera);

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

    std::vector<unsigned char> _raytracingTextureVector;

    void GenerateRGBImage();
    void GenerateBackground();
    void ShootRays();

    void write_color(const color& pixel_color, int index);
    bool hit_sphere(const point3& center, double radius, const Ray& r);

    const uint8_t bytes_per_pixel = 3; 

    color ray_color(const Ray& r);
};