#include "ObjectBase.h"

#include "graphics/materials/Material.h"

ObjectBase::ObjectBase(glm::vec3 center) : _material(std::make_shared<Material>())
{
    Position = center;
}

const Material* ObjectBase::GetMaterial() const
{
    return _material.get();
}
