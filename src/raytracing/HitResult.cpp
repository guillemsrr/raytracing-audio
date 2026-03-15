#include "HitResult.h"

#include "Ray.h"


void HitResult::SetObjectHit(const ObjectBase* object, const Ray& r, const glm::vec3& outwardNormal)
{
    ObjectHit = object;
    _hasHit = true;

    direction = r.direction();
    front_face = dot(direction, outwardNormal) < 0.0f;
    normal = front_face ? outwardNormal : -outwardNormal;
}

bool HitResult::HasHit() const
{
    return _hasHit && ObjectHit != nullptr;
}