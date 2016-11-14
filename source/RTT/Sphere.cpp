#include "Sphere.h"
#include <glm/glm.hpp>

using namespace glm;

Sphere::Sphere(vec3 i, double j, vec3 k, float l)
{
	centre = i;
	radius = j;
	colour = k;
	shininess = l;
}

vec3 Sphere::getNormal()
{
	return pointNormal;
}

bool Sphere::intersect(Ray ray)
{
	vec3 I = centre - ray.origin;
	double tca = dot(I, ray.direction);
	if (tca < 0) return false;

	double s;
	s = sqrt(dot(I, I) - tca*tca);
	if (s > radius) return false;

	double thc = sqrt(radius*radius - s*s);
	double t0 = tca - thc;
	double t1 = tca + thc;

	t = glm::min(t0, t1);

	pointOfIntersection = ray.origin + ray.direction*(float)t0;	// Vec3 is is based on float
	pointNormal = pointOfIntersection - centre;
	return true;
}