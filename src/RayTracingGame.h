// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <memory>

#include "core/GameBase.h"

#include "graphics/Renderer.h"
#include "scene/Scene.h"

#include "audio/AudioEmitter.h"
#include "raytracing/audio/Listener.h"

class IRaytracer;

namespace Light
{
    class PathTracer;
    class WhittedTracer;
}

namespace Audio
{
    class Tracer;
    class Visualizer;
}

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
    Scene* _currentScene;

    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<IRaytracer> _raytracer;

    std::unique_ptr<Light::PathTracer> _lightPathTracer;
    std::unique_ptr<Light::WhittedTracer> _lightWhittedTracer;
    std::unique_ptr<Audio::Visualizer> _audioVisualizer;
    std::unique_ptr<AudioEmitter> _audioEmitter;
    std::unique_ptr<Audio::Listener> _audioListener;
    std::unique_ptr<Audio::Tracer> _audioTracer;
    IRaytracer* _currentRaytracer = nullptr;

    int _audioPathCount = 2000;

    void RenderUI() override;
    void RaytracerRadioButton(const char* label, IRaytracer* raytracer);
    void SceneRadioButton(const char* label, Scene* scene);
    void SetCurrentRaytracer(IRaytracer* raytracer);
    void SetCurrentScene(Scene* scene);
    void ApplySceneToRaytracers();
    void RenderAudioUI();
};
