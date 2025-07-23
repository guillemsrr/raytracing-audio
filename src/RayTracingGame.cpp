#include "RayTracingGame.h"
#include <fstream>

#include "graphics/materials/Material.h"
#include "graphics/materials/MaterialPresets.h"

#include "input/OrbitalCameraInput.h"

#include "objects/SphereObject.h"
#include "objects/ObjectBase.h"

#include <SDL3/SDL_log.h>

RayTracingGame::RayTracingGame(): GameBase(), _renderer(nullptr)
{
}

void RayTracingGame::Init(SDL_Window* window)
{
    GameBase::Init(window);

    SDL_GL_SetSwapInterval(0); // Adaptive vsync

    _camera->SetPitchAngle(3.f);
    _camera->SetRadius(200);
    _camera->SetMaxRadius(400);
    _camera->SetZoomSensitivity(10.f);

    _scene = Scene();
    auto smallSphere = std::make_shared<SphereObject>(point3(0, 0, 0), 0.5f);
    std::shared_ptr<Material> metallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    smallSphere->SetMaterial(metallicMaterial);
    _scene.add(smallSphere);

    auto concreteMaterial = std::make_shared<Material>(MaterialPresets::Concrete());
    auto groundSphere = std::make_shared<SphereObject>(point3(0, -100.5f, 0), 100);
    groundSphere->SetMaterial(concreteMaterial);
    _scene.add(groundSphere);

    _renderer = std::make_unique<Renderer>(_window, _camera);
    _renderer->SetScene(_scene);
    SetRenderer(_renderer.get());

    OrbitalCameraInput* orbitalCameraInput = new OrbitalCameraInput(_camera);
    AddInputHandler(orbitalCameraInput);
}

void RayTracingGame::Update(float deltaTime)
{
    //SDL_Log("Camera pitch angle %f", _camera->GetPitchAngle());

    glm::vec3 center = glm::vec3();
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
    _renderer->RenderRaytracing();
    //_renderer->RenderDebug();
}

void RayTracingGame::Quit()
{
}

void RayTracingGame::RenderUI()
{
}