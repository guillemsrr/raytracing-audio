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
}

void RayTracingGame::HandleEvent(const SDL_Event& e)
{
    GameBase::HandleEvent(e);
}

void RayTracingGame::Render()
{
    glm::vec3 center = glm::vec3();
    _camera->SetTarget(center);
    _camera->UpdatePosition();

    _renderer->RenderBackground();

    glm::vec3 planePosition = glm::vec3(0.f, -0.1f, 0.f);
    float scalevalue = 50.f;
    glm::vec2 scale = glm::vec2(scalevalue, scalevalue);
    //_renderer->RenderPlane(planePosition, scale);

    _renderer->RenderRaytracing();
}

void RayTracingGame::Quit()
{
}

void RayTracingGame::RenderUI()
{
}