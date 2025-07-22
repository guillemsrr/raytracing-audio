#include "RayTracingGame.h"
#include <fstream>
#include <iostream>

#include "input/OrbitalCameraInput.h"

#include "objects/SphereObject.h"

#include <SDL3/SDL_log.h>

RayTracingGame::RayTracingGame(): GameBase(), _renderer(nullptr)
{
}

void RayTracingGame::Init(SDL_Window* window)
{
    GameBase::Init(window);

    _camera->SetPitchAngle(3.f);

    _scene = Scene();
    _scene.add(std::make_shared<SphereObject>(point3(0, 0, 0), 0.1));
    //_scene.add(std::make_shared<SphereObject>(point3(0, 0, -0.1), 0.15));
    _scene.add(std::make_shared<SphereObject>(point3(0, -100.1, 0), 100));

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