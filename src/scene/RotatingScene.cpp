// Copyright (c) Guillem Serra. All Rights Reserved.

#include "RotatingScene.h"

#include <glm/ext/quaternion_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/materials/Material.h"
#include "graphics/materials/MaterialPresets.h"

#include "objects/BoxObject.h"
#include "objects/SphereObject.h"

RotatingScene::RotatingScene()
{
    auto metallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    auto concreteMaterial = std::make_shared<Material>(MaterialPresets::Concrete());
    auto coloredMetallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    coloredMetallicMaterial->Albedo = glm::vec4(0, 0, 1, 1);

    auto box1 = std::make_shared<BoxObject>(vec3(0, 0, 0), vec3(1, 1, 1));
    Add(box1);

    auto smallSphere2 = std::make_shared<SphereObject>(vec3(2, 0, 0), 0.5f);
    smallSphere2->SetMaterial(coloredMetallicMaterial);
    Add(smallSphere2);

    auto groundSphere = std::make_shared<SphereObject>(vec3(0, -100.5f, 0), 100);
    groundSphere->SetMaterial(concreteMaterial);
    Add(groundSphere);
}

void RotatingScene::Update(float deltaTime)
{
    _time += deltaTime;

    float lightX = std::sin(_time * 0.5f);
    float lightZ = std::cos(_time * 0.5f);
    _sunLight.Direction = glm::normalize(glm::vec3(lightX, 1.0f, lightZ));

    if (_objects.size() >= 2)
    {
        _objects[1]->SetCenter(vec3(std::sin(_time) * 3.f, 0, std::cos(_time) * 3.f));

        glm::mat3 rot = glm::mat3(glm::rotate(glm::mat4(1.0f), _time, glm::vec3(0, 1, 0)));
        _objects[0]->SetRotation(rot);
    }
}
