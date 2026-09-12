// Copyright (c) Guillem SerraEngine. All Rights Reserved.

#include "RayTracingGame.h"

#include <filesystem>

#include "core/App.h"

using namespace SerraEngine;

int main(int argc, char* argv[])
{
    const char* basePath = SDL_GetBasePath();
    if (!basePath)
    {
        SDL_Log("Failed to locate runtime assets: %s", SDL_GetError());
        return 1;
    }

    std::error_code pathError;
    std::filesystem::current_path(basePath, pathError);
    if (pathError)
    {
        SDL_Log("Failed to use runtime directory '%s': %s", basePath, pathError.message().c_str());
        return 1;
    }

    App EngineApp = App(argc, argv);
    if (!EngineApp.Init("Raytracing + audio", 960, 600))
    {
        return 0;
    }

    RayTracingGame game;
    EngineApp.StartGame(game);

    return 0;
}
