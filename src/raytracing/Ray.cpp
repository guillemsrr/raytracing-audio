#include "Ray.h"

#include <glm/detail/func_geometric.inl>

Ray::Ray(const point3& origin, const vec3& direction) : orig(origin)
{
    dir = glm::normalize(direction);
}