#include "ObjectBase.h"

#include "graphics/materials/Material.h"
#include <glm/mat3x3.hpp>

ObjectBase::ObjectBase(glm::vec3 center) : _center(center), _rotation(glm::mat3(1.0f)),
                                           _material(std::make_shared<Material>())
{
}

const Material* ObjectBase::GetMaterial() const
{
    return _material.get();
}

const glm::vec3& ObjectBase::GetCenter() const
{
    return _center;
}
