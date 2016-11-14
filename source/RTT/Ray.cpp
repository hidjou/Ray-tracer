#include "Ray.h"
#include <glm/glm.hpp>

using namespace glm;

Ray::Ray(vec3 i, vec3 j)
{
	origin = i;
	direction = j;
}