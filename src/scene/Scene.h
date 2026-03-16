// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

class Interval;
class Ray;
class ObjectBase;
struct HitResult;

struct DirectionalLight
{
    glm::vec3 Direction = glm::normalize(glm::vec3(-0.8f, 1.1f, -0.5f));
    glm::vec3 Color = glm::vec3(1.0f, 0.98f, 0.95f);
    float Intensity = 1.35f;
};

class Scene
{
public:
    Scene() = default;

    void Add(std::shared_ptr<ObjectBase> object);

    HitResult HitAny(Ray ray, Interval ray_t) const;

    const DirectionalLight& GetSunLight() const { return _sunLight; }
    const glm::vec3& GetAmbientLight() const { return _ambientLight; }
    const glm::vec3& GetSkyHorizonColor() const { return _skyHorizonColor; }
    const glm::vec3& GetSkyZenithColor() const { return _skyZenithColor; }

protected:
    std::vector<std::shared_ptr<ObjectBase>> _objects;
    DirectionalLight _sunLight;
    glm::vec3 _ambientLight = glm::vec3(0.08f, 0.09f, 0.12f);
    glm::vec3 _skyHorizonColor = glm::vec3(0.92f, 0.95f, 1.0f);
    glm::vec3 _skyZenithColor = glm::vec3(0.28f, 0.41f, 0.63f);
};