// Copyright (c) Guillem Serra. All Rights Reserved.

#include "RayTracingGame.h"
#include <algorithm>
#include <fstream>

#include "input/OrbitalCameraInput.h"

#include "objects/ObjectBase.h"

#include <SDL3/SDL_log.h>

#include "raytracing/base/IRaytracer.h"
#include "raytracing/audio/Tracer.h"
#include "raytracing/audio/Visualizer.h"
#include "raytracing/light/PathTracer.h"
#include "raytracing/light/WhittedTracer.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "scene/MovingObjectsScene.h"
#include "scene/RotatingScene.h"
#include "scene/StaticScene.h"

using vec3 = glm::vec3;

RayTracingGame::RayTracingGame() = default;
RayTracingGame::~RayTracingGame() = default;

void RayTracingGame::Init(SDL_Window* window)
{
    GameBase::Init(window);

    SDL_GL_SetSwapInterval(0); // Adaptive vsync

    _camera->SetPitchAngle(15.f);
    _camera->SetRadius(5.f);
    _camera->SetMaxRadius(200.f);

    _scenes.push_back(std::make_unique<StaticScene>());
    _scenes.push_back(std::make_unique<MovingObjectsScene>());
    _scenes.push_back(std::make_unique<RotatingScene>());

    _currentScene = _scenes[0].get();

    _renderer = std::make_unique<Renderer>(_window, _camera);
    _lightPathTracer = std::make_unique<Light::PathTracer>(_camera);
    _lightWhittedTracer = std::make_unique<Light::WhittedTracer>(_camera);
    _audioEmitter = std::make_unique<AudioEmitter>();
    _audioEmitter->Position = vec3(0, 5, 0);
    _audioListener = std::make_unique<Audio::Listener>();
    _audioTracer = std::make_unique<Audio::Tracer>();
    _audioTracer->SetEmitter(_audioEmitter.get());
    _audioTracer->SetListener(_audioListener.get());
    _audioVisualizer = std::make_unique<Audio::Visualizer>(_camera);
    _audioVisualizer->SetEmitter(_audioEmitter.get());

    _currentRaytracer = _lightPathTracer.get();

    ApplySceneToRaytracers();

    SetRenderer(_renderer.get());

    const auto orbitalCameraInput = new OrbitalCameraInput(_camera);
    AddInputHandler(orbitalCameraInput);
}

void RayTracingGame::Update(float deltaTime)
{
    //SDL_Log("Camera pitch angle %f", _camera->GetPitchAngle());

    const auto center = glm::vec3();
    _camera->SetTarget(center);
    _camera->UpdatePosition();
    _currentScene->Update(deltaTime);
}

void RayTracingGame::HandleEvent(const SDL_Event& e)
{
    GameBase::HandleEvent(e);
}

void RayTracingGame::Render()
{
    _renderer->RenderBackground();
    _renderer->UpdateAndRender(_currentRaytracer);

    //_currentRaytracer->ResetAccumulation();
    //_renderer->RenderDebug();
}

void RayTracingGame::Quit()
{
}

void RayTracingGame::RenderUI()
{
    ImGui::Text("Light Tracer:");
    RaytracerRadioButton("Stochastic Path Tracer (Accumulation)", _lightPathTracer.get());
    RaytracerRadioButton("Whitted Raytracer (Direct + Simple Reflections)", _lightWhittedTracer.get());
    RaytracerRadioButton("Sound Visualizer (Acoustic Energy per Band)", _audioVisualizer.get());

    ImGui::Separator();
    ImGui::Text("Scene Selection:");
    SceneRadioButton("Static Scene", _scenes[0].get());
    SceneRadioButton("Moving Objects Scene", _scenes[1].get());
    SceneRadioButton("Rotating Light & Objects Scene", _scenes[2].get());

    ImGui::Separator();
    RenderAudioUI();
}

void RayTracingGame::RaytracerRadioButton(const char* label, IRaytracer* raytracer)
{
    if (ImGui::RadioButton(label, _currentRaytracer == raytracer))
    {
        SetCurrentRaytracer(raytracer);
    }
}

void RayTracingGame::SceneRadioButton(const char* label, Scene* scene)
{
    if (ImGui::RadioButton(label, _currentScene == scene))
    {
        SetCurrentScene(scene);
    }
}

void RayTracingGame::SetCurrentRaytracer(IRaytracer* raytracer)
{
    _currentRaytracer = raytracer;
    _currentRaytracer->ResetAccumulation();
}

void RayTracingGame::SetCurrentScene(Scene* scene)
{
    _currentScene = scene;
    ApplySceneToRaytracers();
}

void RayTracingGame::ApplySceneToRaytracers()
{
    _lightPathTracer->SetScene(*_currentScene);
    _lightWhittedTracer->SetScene(*_currentScene);
    _audioTracer->SetScene(*_currentScene);
    _audioVisualizer->SetScene(*_currentScene);
}

void RayTracingGame::RenderAudioUI()
{
    ImGui::Text("Sound:");
    ImGui::SliderInt("Paths", &_audioPathCount, 100, 20000);
    if (ImGui::Button("Trace impulse response"))
    {
        _audioListener->Position = _camera->GetPosition();
        _audioTracer->Trace(_audioPathCount);
    }
}
