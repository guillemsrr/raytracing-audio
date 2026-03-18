// Copyright (c) Guillem Serra. All Rights Reserved.

#include "MovingObjectsScene.h"

#include <memory>

#include "graphics/materials/Material.h"
#include "graphics/materials/MaterialPresets.h"
#include "objects/SphereObject.h"


MovingObjectsScene::MovingObjectsScene()
{
    auto metallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    auto concreteMaterial = std::make_shared<Material>(MaterialPresets::Concrete());
    auto coloredMetallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    coloredMetallicMaterial->Albedo = glm::vec4(0, 1, 0, 1);

    auto smallSphere = std::make_shared<SphereObject>(vec3(0, 0, 0), 0.5f);
    smallSphere->SetMaterial(metallicMaterial);
    Add(smallSphere);

    auto smallSphere2 = std::make_shared<SphereObject>(vec3(1, 0, 0), 0.5f);
    smallSphere2->SetMaterial(coloredMetallicMaterial);
    Add(smallSphere2);

    auto groundSphere = std::make_shared<SphereObject>(vec3(0, -100.5f, 0), 100);
    groundSphere->SetMaterial(concreteMaterial);
    Add(groundSphere);
}

void MovingObjectsScene::Update(float deltaTime)
{
    _time += deltaTime;
    if (_objects.size() >= 2)
    {
        _objects[0]->SetPosition(vec3(std::sin(_time) * 1.2f, 0, std::cos(_time) * 1.2f));
        _objects[1]->SetPosition(vec3(std::cos(_time* 2.f) * 2.f, 0, std::sin(_time* 2.f) * 2.f));
    }
}
