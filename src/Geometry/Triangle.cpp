#include "Triangle.h"

using namespace glm;

Triangle::Triangle(vec3 v1, vec3 v2, vec3 v3, vec3 _normal, vec3 _color) :
	vertices{ v1,v2,v3 }, normal(_normal), color(_color) {

}

Triangle::Triangle(vec3 _vertices[3], vec3 _normal, vec3 _color) :
	normal(_normal), color(_color) {
	for (int i = 0; i < 3; ++i) { vertices[i] = _vertices[i]; }
}

// Implementation using the Möller-Trumbore (MT) ray intersection algorithm.
bool Triangle::RayIntersection(const Ray & ray, vec3 & intersectionPoint) {
	if (dot(ray.dir, normal) > -FLT_EPSILON) {
		return false; // The ray direction and normal are in the same direction. 
	}

	// Calculate intersection using barycentric coordinates. This gives a equation system
	// which we can solve using Cramer's rule.
	// See http://staffwww.itn.liu.se/~mardi/WebPages/Courses/TNCG15/TheCode.pdf for more information.
	const vec3 T = ray.from - vertices[0];
	const vec3 E1 = vertices[1] - vertices[0];
	const vec3 E2 = vertices[2] - vertices[0];
	const vec3 D = ray.to - ray.from;
	const vec3 P = cross(D, E2);
	const vec3 Q = cross(T, E1);
	const float den = dot(P, E1);
	if (den < FLT_EPSILON) {
		return false; // Didn't hit.
	}
	const float t = dot(Q, E2) / den;

	// Calculate answer.
	intersectionPoint = ray.from + t * D;
	return true;
}
