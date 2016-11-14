#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Ray.h"
#include "Shape.h"
#include <glm/glm.hpp>

using namespace glm;

class Triangle : public Shape
{
public:
	vec3 v0;
	vec3 v1;
	vec3 v2;
	vec3 n0;
	vec3 n1;
	vec3 n2;

	Triangle() = default;
	Triangle(vec3 i, vec3 j, vec3 k, vec3 l, vec3 m, vec3 n, vec3 o, double p);

	bool intersect(Ray ray);
	vec3 getNormal();

	float u, v, w;

};

#endif