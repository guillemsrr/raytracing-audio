// Copyright (c) Guillem SerraEngine. All Rights Reserved.

#include "RayTracingGame.h"

#include "core/App.h"

using namespace SerraEngine;

int main(int argc, char* argv[])
{
    App EngineApp = App(argc, argv);
    if (!EngineApp.Init("Raytracing + audio", 800, 800))
    {
        return 0;
    }

    RayTracingGame game;
    EngineApp.StartGame(game);

    return 0;
}