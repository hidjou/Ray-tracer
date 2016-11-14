#ifndef BOX_H
#define BOX_H

#include "Shape.h"
#include <glm/glm.hpp>


using namespace glm;

class Box
{
public:
	vec3 position, size;

	Box() = default;
	Box(vec3 i, vec3 j);
};
#endif