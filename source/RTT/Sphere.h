#ifndef SPHERE_H
#define SPHERE_H

#include "Ray.h"
#include "Shape.h"
#include <glm/glm.hpp>

using namespace glm;

class Sphere : public Shape
{
public:
	Sphere() = default;
	Sphere(vec3 i, double j, vec3 k, float l);

	vec3 centre;
	double radius;

	bool intersect(Ray ray);
	vec3 getNormal();
};

#endif