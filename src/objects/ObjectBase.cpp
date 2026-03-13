#include "ObjectBase.h"

#include "graphics/materials/Material.h"
#include <glm/mat3x3.hpp>

/*void ObjectBase::SetMaterial(const std::shared_ptr<Material>& material)
{
    _material = material;
}*/


ObjectBase::ObjectBase(glm::vec3 center) : _center(center), _rotation(glm::mat3(1.0f)),
                                           _material(std::make_shared<Material>())
{
}

const Material* ObjectBase::GetMaterial() const
{
    return _material.get();
}