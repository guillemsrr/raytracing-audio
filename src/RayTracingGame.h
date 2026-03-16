// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <memory>

#include "core/GameBase.h"

#include "graphics/Renderer.h"
#include "scene/Scene.h"

class LightRaytracer;
class IRaytracer;

class RayTracingGame final : public SerraEngine::GameBase
{
public:
    RayTracingGame();
    ~RayTracingGame() override;
    void Init(SDL_Window* window) override;
    void Update(float deltaTime) override;
    void HandleEvent(const SDL_Event& e) override;
    void Render() override;
    void Quit() override;

private:
    Scene _scene;

    void RenderUI() override;

    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<IRaytracer> _raytracer;
};
