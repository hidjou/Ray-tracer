#include "Plane.h"
#include "glm/glm.hpp"

using namespace glm;

Plane::Plane(vec3 i, vec3 j, vec3 k, double l)
{
	position = i;
	normal = j;
	colour = k;
	shininess = l;
}

bool Plane::intersect(Ray ray)
{
	float denom = dot(normal, ray.direction);
	if (abs(denom) > 1e-6)
	{
		vec3 p0l0 = position - ray.origin;
		t = dot(p0l0, normal) / denom;
		pointOfIntersection = ray.origin + ray.direction * (float)t;
		return(t >= 0);
	}
	return false;
}

vec3 Plane::getNormal()
{
	return normal;
}