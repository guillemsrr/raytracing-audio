// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "IRaytracer.h"
#include "graphics/Camera.h"
#include "raytracing/Ray.h"
#include "scene/Scene.h"

using color = glm::vec4;

class RaytracerBase : public IRaytracer
{
public:
    explicit RaytracerBase(Camera* camera);
    ~RaytracerBase() override = default;

    void SetScene(const Scene& scene) override;
    void Resize(int width, int height) override;
    void Render() override;
    void ResetAccumulation() override;

    [[nodiscard]] int GetWidth() const override;
    [[nodiscard]] int GetHeight() const override;
    [[nodiscard]] const uint32_t* GetColorBuffer() const override;

protected:
    virtual color TraceRay(const Ray& ray) const = 0;
    virtual bool ShouldAccumulate() const { return false; }

    void RenderPixel(const glm::vec3& pixelPosition, int index);

    float _shadowBias = 0.02f;

    Camera* _camera = nullptr;
    const Scene* _scene = nullptr;

    int _width = 0;
    int _height = 0;
    uint32_t _frameIndex = 1;

    std::vector<uint32_t> _colorBuffer;
    std::vector<color> _accumulation;
    std::vector<int> _pixelScreenHorizontalIterator;
    std::vector<int> _pixelScreenVerticalIterator;
};
