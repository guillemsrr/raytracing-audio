#include "ObjectBase.h"

#include "graphics/materials/Material.h"

ObjectBase::ObjectBase() : _material(std::make_shared<Material>())
{
}

/*void ObjectBase::SetMaterial(const std::shared_ptr<Material>& material)
{
    _material = material;
}*/

const Material* ObjectBase::GetMaterial() const
{
    return _material.get();
}