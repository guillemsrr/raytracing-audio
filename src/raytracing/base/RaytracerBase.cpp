// Copyright (c) Guillem Serra. All Rights Reserved.

#include "RaytracerBase.h"
#include <algorithm>
#include <execution>
#include <cmath>

#include "Ray.h"
#include "utils/Utils.h"

RaytracerBase::RaytracerBase(Camera* camera) : _camera(camera)
{
    _camera->AddOnCameraMovedListener([this]()
    {
        ResetAccumulation();
    });
}

void RaytracerBase::SetScene(const Scene& scene)
{
    _scene = &scene;
    ResetAccumulation();
}

void RaytracerBase::Resize(int width, int height)
{
    if (width <= 0 || height <= 0 || (width == _width && height == _height))
    {
        return;
    }

    _width = width;
    _height = height;

    const size_t pixelCount = static_cast<size_t>(_width) * static_cast<size_t>(_height);
    _colorBuffer.resize(pixelCount);
    _accumulation.resize(pixelCount);

    _pixelScreenHorizontalIterator.resize(_width);
    _pixelScreenVerticalIterator.resize(_height);

    for (int i = 0; i < _width; ++i)
    {
        _pixelScreenHorizontalIterator[i] = i;
    }

    for (int i = 0; i < _height; ++i)
    {
        _pixelScreenVerticalIterator[i] = i;
    }

    ResetAccumulation();
}

void RaytracerBase::Render()
{
    if (!_scene || _width <= 0 || _height <= 0)
    {
        return;
    }

    if (_frameIndex == 1 || !ShouldAccumulate())
    {
        std::memset(_accumulation.data(), 0, sizeof(color) * _accumulation.size());
    }

    const float aspect = _camera->GetAspectRatio();
    const float fovRad = _camera->GetFOVRad();
    const float viewportHeight = 2.0f * std::tan(fovRad * 0.5f);
    const float viewportWidth = viewportHeight * aspect;

    const glm::vec3 horizontal = viewportWidth * _camera->GetRight();
    const glm::vec3 vertical = viewportHeight * _camera->GetUp();
    const glm::vec3 lowerLeftCorner = _camera->GetPosition() + _camera->GetForward()
        - horizontal * 0.5f - vertical * 0.5f;

    std::for_each(std::execution::par,
                  _pixelScreenVerticalIterator.begin(),
                  _pixelScreenVerticalIterator.end(),
                  [&](int j)
                  {
                      for (const int i : _pixelScreenHorizontalIterator)
                      {
                          const float jitterX = ShouldAccumulate() ? Utils::RandomFloat() : 0.5f;
                          const float jitterY = ShouldAccumulate() ? Utils::RandomFloat() : 0.5f;
                          const float u = (static_cast<float>(i) + jitterX) / static_cast<float>(_width);
                          const float v = (static_cast<float>(j) + jitterY) / static_cast<float>(_height);
                          const glm::vec3 pixelPosition = lowerLeftCorner + u * horizontal + v * vertical;
                          const int index = j * _width + i;
                          RenderPixel(pixelPosition, index);
                      }
                  });

    if (ShouldAccumulate())
    {
        _frameIndex++;
    }
}

void RaytracerBase::ResetAccumulation()
{
    _frameIndex = 1;
}

int RaytracerBase::GetWidth() const
{
    return _width;
}

int RaytracerBase::GetHeight() const
{
    return _height;
}

const uint32_t* RaytracerBase::GetColorBuffer() const
{
    return _colorBuffer.empty() ? nullptr : _colorBuffer.data();
}

void RaytracerBase::RenderPixel(const glm::vec3& pixelPosition, int index)
{
    const glm::vec3 rayOrigin = _camera->GetPosition();
    const glm::vec3 rayDirection = glm::normalize(pixelPosition - rayOrigin);
    const color pixelColor = TraceRay(Ray(rayOrigin, rayDirection));

    if (!ShouldAccumulate())
    {
        _accumulation[index] = pixelColor;
    }
    else
    {
        _accumulation[index] += pixelColor;
    }

    color accumulatedColor = !ShouldAccumulate()
                                 ? _accumulation[index]
                                 : _accumulation[index] / static_cast<float>(_frameIndex);
    accumulatedColor = glm::max(accumulatedColor, color(0.0f));
    accumulatedColor.r = std::sqrt(std::min(accumulatedColor.r, 1.0f));
    accumulatedColor.g = std::sqrt(std::min(accumulatedColor.g, 1.0f));
    accumulatedColor.b = std::sqrt(std::min(accumulatedColor.b, 1.0f));
    accumulatedColor.a = 1.0f;

    _colorBuffer[index] = Utils::ConvertToRGBA(accumulatedColor);
}
