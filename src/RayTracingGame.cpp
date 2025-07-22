#include "RayTracingGame.h"
#include <fstream>
#include <iostream>

#include "input/OrbitalCameraInput.h"

RayTracingGame::RayTracingGame(): GameBase(), _renderer(nullptr)
{
}

void RayTracingGame::Init(SDL_Window* window)
{
    GameBase::Init(window);

    glm::vec3 cameraPosition = glm::vec3(0, 5, 5);
    _camera->SetPosition(cameraPosition);

    _renderer = std::make_unique<Renderer>(_window, _camera);
    SetRenderer(_renderer.get());

    OrbitalCameraInput* orbitalCameraInput = new OrbitalCameraInput(_camera);
    AddInputHandler(orbitalCameraInput);
}

void RayTracingGame::Update(float deltaTime)
{
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