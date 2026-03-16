#include "RayTracingGame.h"
#include <fstream>

#include "graphics/materials/Material.h"
#include "graphics/materials/MaterialPresets.h"

#include "input/OrbitalCameraInput.h"

#include "objects/SphereObject.h"
#include "objects/ObjectBase.h"

#include <SDL3/SDL_log.h>

#include "objects/BoxObject.h"
#include "raytracing/IRaytracer.h"
#include "raytracing/PathRaytracer.h"
#include "raytracing/WhittedRaytracer.h"

#include <imgui.h>

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

    _scene = Scene();
    auto metallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    auto concreteMaterial = std::make_shared<Material>(MaterialPresets::Concrete());
    auto coloredMetallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    coloredMetallicMaterial->Albedo = glm::vec4(1, 0, 0, 1);

    auto smallSphere = std::make_shared<SphereObject>(vec3(0, 0, 0), 0.5f);
    smallSphere->SetMaterial(metallicMaterial);
    _scene.Add(smallSphere);

    auto smallSphere2 = std::make_shared<SphereObject>(vec3(1, 0, 0), 0.5f);
    smallSphere2->SetMaterial(coloredMetallicMaterial);
    _scene.Add(smallSphere2);

    auto box1 = std::make_shared<BoxObject>(vec3(2, 0, 0), vec3(1, 1, 1));
    _scene.Add(box1);

    auto groundSphere = std::make_shared<SphereObject>(vec3(0, -100.5f, 0), 100);
    groundSphere->SetMaterial(concreteMaterial);
    _scene.Add(groundSphere);

    _renderer = std::make_unique<Renderer>(_window, _camera);
    _pathRaytracer = std::make_unique<PathRaytracer>(_camera);
    _pathRaytracer->SetScene(_scene);

    _whittedRaytracer = std::make_unique<WhittedRaytracer>(_camera);
    _whittedRaytracer->SetScene(_scene);

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
    ImGui::Begin("Raytracing Mode");

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

    ImGui::PopStyleColor();
    ImGui::End();
}
