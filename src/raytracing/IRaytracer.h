// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include <cstdint>

class Scene;

class IRaytracer
{
public:
    virtual ~IRaytracer() = default;

    virtual void SetScene(const Scene& scene) = 0;
    virtual void Resize(int width, int height) = 0;
    virtual void Render() = 0;
    virtual void ResetAccumulation() = 0;

    [[nodiscard]] virtual int GetWidth() const = 0;
    [[nodiscard]] virtual int GetHeight() const = 0;
    [[nodiscard]] virtual const uint32_t* GetColorBuffer() const = 0;
};
