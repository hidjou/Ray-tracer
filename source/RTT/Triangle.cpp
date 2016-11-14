#include "Triangle.h"
#include <iostream>
#include "glm/glm.hpp"

using namespace glm;

Triangle::Triangle(vec3 i, vec3 j, vec3 k, vec3 l, vec3 m, vec3 n, vec3 o, double p)
{
	colour = i;
	v0 = j;
	v1 = k;
	v2 = l;
	n0 = m;
	n1 = n;
	n2 = o;
	shininess = p;
}

vec3 Triangle::getNormal()
{
	return pointNormal;
}

bool Triangle::intersect(Ray ray)
{
	// Triangle edges
	vec3 edge0 = v1 - v0;
	vec3 edge1 = v2 - v0;
	vec3 edge2 = v0 - v2;

	//// Plane normal
	//vec3 N = cross(edge0, edge1);

	//// Finding P
	//float NdotRayDirection = dot(N, ray.direction);
	//if (abs(NdotRayDirection) < 1e-6)
	//	return false;	// Plane and ray parallel = no intersection

	//float d = dot(N, v0);
	//t = (dot(N, ray.origin) + d) / NdotRayDirection;
	//if (t < 0) return false;	// Triangle is behind the ray

	//vec3 P = ray.origin + t*ray.direction;	// P = intersection point

	//vec3 C;

	//// The "left side" test
	//vec3 vp0 = P - v0;
	//C = cross(edge0, vp0);
	//if (dot(N, C) < 0) return false;

	//vec3 vp1 = P - v1;
	//C = cross(edge1, vp1);
	//if (dot(N, C) < 0) return false;

	//vec3 vp2 = P - v2;
	//C = cross(edge2, vp2);
	//if (dot(N, C) < 0) return false;

	//return true;

	u = (dot(ray.origin - v0, cross(ray.direction, edge1))) 
		/ dot(edge0, cross(ray.direction, edge1));

	v = (dot(ray.direction, cross((ray.origin - v0), edge0))) 
		/ dot(edge0, cross(ray.direction, edge1));

	/*float w = (dot(ray.origin - v0, cross(ray.direction, edge2)))
		/ dot(edge0, cross(ray.direction, edge2));*/

	w = 1 - u - v;

	pointNormal = w * n0 + u * n1 + v * n2;

	t = (dot(edge1, cross((ray.origin - v0), edge0))) / dot(edge0, cross(ray.direction, edge1));
	pointOfIntersection = ray.origin + (float)t * ray.direction;

	if ((u < 0) || (u > 1))
		return false;
	if ((v < 0) || ((u + v) > 1))
		return false;

	return true;
}