#include "RayTracingGame.h"

#include "input/OrbitalCameraInput.h"

RayTracingGame::RayTracingGame(): GameBase(), _renderer(nullptr)
{
}

void RayTracingGame::Init(SDL_Window* window)
{
    GameBase::Init(window);

    glm::vec3 cameraPosition = glm::vec3(0, 5, 5);
    _camera->SetPosition(cameraPosition);

    _renderer = Renderer(_camera);
    _rendererBase = _renderer;

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

    _renderer.RenderBackground();
    _renderer.RenderPlane();
}

void RayTracingGame::Quit()
{
}

void RayTracingGame::RenderUI()
{
}