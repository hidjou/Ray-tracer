#ifndef SHAPE_H
#define SHAPE_H

#include <glm/glm.hpp>
#include "Ray.h"

using namespace glm;

class Shape
{
public:
	vec3 position;
	vec3 normal;
	vec3 colour;
	double shininess;

	vec3 pointNormal;

	double t;
	vec3 pointOfIntersection;

	Shape() = default;
	Shape(vec3 i, vec3 j, vec3 k, double l);

	virtual vec3 getNormal();

	virtual bool intersect(Ray ray);
};

#endif