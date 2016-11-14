#include "Box.h"
#include <glm/glm.hpp>

using namespace glm;

Box::Box(vec3 i, vec3 j)
{
	position = i;
	size = j;
}