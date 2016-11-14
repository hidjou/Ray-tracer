#ifndef PLANE_H
#define PLANE_H

#include "Ray.h"
#include "Shape.h"
#include <glm/glm.hpp>

using namespace glm;

class Plane : public Shape
{
public:
	Plane() = default;
	Plane(vec3 i, vec3 j, vec3 k, double l);

	bool intersect(Ray ray);
	vec3 getNormal();
};

#endif