// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <memory>

#include "core/GameBase.h"

#include "graphics/Renderer.h"
#include "scene/Scene.h"

class PathRaytracer;
class WhittedRaytracer;
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
    std::vector<std::unique_ptr<Scene>> _scenes;
    int _currentSceneIndex = 0;

    void RenderUI() override;

    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<IRaytracer> _raytracer;

    std::unique_ptr<PathRaytracer> _pathRaytracer;
    std::unique_ptr<WhittedRaytracer> _whittedRaytracer;
    IRaytracer* _currentRaytracer = nullptr;
};
