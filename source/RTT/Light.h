#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

using namespace glm;

class Light
{
public:
	vec3 centre;
	vec3 colour;
	float intensity;

	Light() = default;
	Light(vec3 i, vec3 j, float k);
};

#endif