// Copyright (c) Guillem Serra. All Rights Reserved.

#include "StaticScene.h"

#include "graphics/materials/Material.h"
#include "graphics/materials/MaterialPresets.h"
#include "objects/BoxObject.h"
#include "objects/SphereObject.h"


StaticScene::StaticScene()
{
    auto metallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    auto concreteMaterial = std::make_shared<Material>(MaterialPresets::Concrete());
    auto coloredMetallicMaterial = std::make_shared<Material>(MaterialPresets::Metallic());
    coloredMetallicMaterial->Albedo = glm::vec4(1, 0, 0, 1);

    auto smallSphere = std::make_shared<SphereObject>(vec3(0, 0, 0), 0.5f);
    smallSphere->SetMaterial(metallicMaterial);
    Add(smallSphere);

    auto smallSphere2 = std::make_shared<SphereObject>(vec3(1, 0, 0), 0.5f);
    smallSphere2->SetMaterial(coloredMetallicMaterial);
    Add(smallSphere2);

    auto box1 = std::make_shared<BoxObject>(vec3(2, 0, 0), vec3(1, 1, 1));
    Add(box1);

    auto groundSphere = std::make_shared<SphereObject>(vec3(0, -100.5f, 0), 100);
    groundSphere->SetMaterial(concreteMaterial);
    Add(groundSphere);
}
