#include "HitResult.h"

#include "Ray.h"

void HitResult::SetObjectHit(ObjectBase* object, const Ray& r)
{
    this->ObjectHit = object;
    _hasHit = true;

    direction = r.direction();
    front_face = dot(direction, -normal) < 0;
    normal = front_face ? -normal : normal;
}

bool HitResult::HasHit() const
{
    return _hasHit && ObjectHit != nullptr;
}