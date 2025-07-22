#include "HitResult.h"

#include "Ray.h"

void HitResult::SetObjectHit(ObjectBase* object, const Ray& r, const glm::vec3& outward_normal)
{
    this->ObjectHit = object;
    _hasHit = true;

    direction = r.direction();
    front_face = dot(direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

bool HitResult::HasHit() const
{
    return _hasHit && ObjectHit != nullptr;
}