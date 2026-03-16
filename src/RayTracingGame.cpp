#include "RayTracingGame.h"
#include <fstream>

#include "input/OrbitalCameraInput.h"

#include "objects/SphereObject.h"
#include "objects/ObjectBase.h"

#include <SDL3/SDL_log.h>

#include "raytracing/IRaytracer.h"
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

    _currentSceneIndex = 0;

    _renderer = std::make_unique<Renderer>(_window, _camera);
    _pathRaytracer = std::make_unique<PathRaytracer>(_camera);
    _pathRaytracer->SetScene(*_scenes[_currentSceneIndex]);

    _whittedRaytracer = std::make_unique<WhittedRaytracer>(_camera);
    _whittedRaytracer->SetScene(*_scenes[_currentSceneIndex]);

    _currentRaytracer = _pathRaytracer.get();

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

    if (_currentSceneIndex > 0)
    {
        _scenes[_currentSceneIndex]->Update(deltaTime);
        _currentRaytracer->ResetAccumulation();
    }
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

void RayTracingGame::RenderUI()
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::Begin("Raytracing Settings");

    ImGui::Text("Raytracing Mode:");
    static int mode = 0; // 0 for Path, 1 for Whitted
    if (ImGui::RadioButton("Stochastic Path Tracer (Accumulation)", &mode, 0))
    {
        _currentRaytracer = _pathRaytracer.get();
        _currentRaytracer->ResetAccumulation();
    }

    if (ImGui::RadioButton("Whitted Raytracer (Direct + Simple Reflections)", &mode, 1))
    {
        _currentRaytracer = _whittedRaytracer.get();
        _currentRaytracer->ResetAccumulation();
    }

    ImGui::Separator();
    ImGui::Text("Scene Selection:");
    int previousSceneIndex = _currentSceneIndex;

    if (ImGui::RadioButton("Static Scene", &_currentSceneIndex, 0))
    {
    }
    if (ImGui::RadioButton("Moving Objects Scene", &_currentSceneIndex, 1))
    {
    }
    if (ImGui::RadioButton("Rotating Light & Objects Scene", &_currentSceneIndex, 2))
    {
    }

    if (previousSceneIndex != _currentSceneIndex)
    {
        _pathRaytracer->SetScene(*_scenes[_currentSceneIndex]);
        _whittedRaytracer->SetScene(*_scenes[_currentSceneIndex]);
        _currentRaytracer->ResetAccumulation();
    }

    ImGui::PopStyleColor();
    ImGui::End();
}
