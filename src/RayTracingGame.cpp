#include "RayTracingGame.h"
#include <fstream>

#include "input/OrbitalCameraInput.h"

#include "objects/ObjectBase.h"

#include <SDL3/SDL_log.h>

#include "raytracing/IRaytracer.h"
#include "raytracing/AudioTracer.h"
#include "raytracing/PathRaytracer.h"
#include "raytracing/WhittedRaytracer.h"

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

    _camera->SetPitchAngle(3.f);
    _camera->SetRadius(200);
    _camera->SetMaxRadius(400);
    _camera->SetZoomSensitivity(10.f);

    _scenes.push_back(std::make_unique<StaticScene>());
    _scenes.push_back(std::make_unique<MovingObjectsScene>());
    _scenes.push_back(std::make_unique<RotatingScene>());

    _currentScene = _scenes[0].get();

    _renderer = std::make_unique<Renderer>(_window, _camera);
    _pathRaytracer = std::make_unique<PathRaytracer>(_camera);
    _whittedRaytracer = std::make_unique<WhittedRaytracer>(_camera);
    _audioTracer = std::make_unique<AudioTracer>(_camera);

    _currentRaytracer = _pathRaytracer.get();

    ApplySceneToRaytracers();

    SetRenderer(_renderer.get());

    auto orbitalCameraInput = new OrbitalCameraInput(_camera);
    AddInputHandler(orbitalCameraInput);
}

void RayTracingGame::Update(float deltaTime)
{
    //SDL_Log("Camera pitch angle %f", _camera->GetPitchAngle());

    auto center = glm::vec3();
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
    //_renderer->RenderDebug();
}

void RayTracingGame::Quit()
{
}

void RayTracingGame::ApplySceneToRaytracers()
{
    _pathRaytracer->SetScene(*_currentScene);
    _whittedRaytracer->SetScene(*_currentScene);
    _audioTracer->SetScene(*_currentScene);
}

bool RayTracingGame::IsAudioMode() const
{
    return _currentRaytracer == _audioTracer.get();
}


void RayTracingGame::RenderUI()
{
    ImGui::Text("Mode: %s", IsAudioMode() ? "Audio" : "Light");
    ImGui::Separator();

    ImGui::Text("Light Tracer:");
    if (ImGui::RadioButton("Stochastic Path Tracer (Accumulation)", _currentRaytracer == _pathRaytracer.get()))
    {
        _currentRaytracer = _pathRaytracer.get();
        _currentRaytracer->ResetAccumulation();
    }

    if (ImGui::RadioButton("Whitted Raytracer (Direct + Simple Reflections)",
                           _currentRaytracer == _whittedRaytracer.get()))
    {
        _currentRaytracer = _whittedRaytracer.get();
    }

    ImGui::Separator();
    ImGui::Text("Scene Selection:");

    if (ImGui::RadioButton("Static Scene", _currentScene == _scenes[0].get()))
    {
        _currentScene = _scenes[0].get();
        ApplySceneToRaytracers();
    }
    if (ImGui::RadioButton("Moving Objects Scene", _currentScene == _scenes[1].get()))
    {
        _currentScene = _scenes[1].get();
        ApplySceneToRaytracers();
    }
    if (ImGui::RadioButton("Rotating Light & Objects Scene", _currentScene == _scenes[2].get()))
    {
        _currentScene = _scenes[2].get();
        ApplySceneToRaytracers();
    }
}
