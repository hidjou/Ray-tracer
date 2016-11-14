#include "Light.h"
#include <glm/glm.hpp>

using namespace glm;

Light::Light(vec3 i, vec3 j, float k) 
{
	centre = i;
	colour = j;
	intensity = k;
}