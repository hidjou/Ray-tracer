#include "Shape.h"
#include <iostream>
#include "glm/glm.hpp"

using namespace glm;

Shape::Shape(vec3 i, vec3 j, vec3 k, double l)
{
	position = i;
	normal = j;
	colour = k;
	shininess = l;
}

bool Shape::intersect(Ray ray)
{
	std::cout << "shape intersect" << std::endl;
	return false;
}

vec3 Shape::getNormal()
{
	std::cout << "shape getnormal" << std::endl;
	return pointNormal;
}