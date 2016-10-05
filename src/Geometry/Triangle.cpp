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

	const auto& v0 = vertices[0];
	const auto& v1 = vertices[1];
	const auto& v2 = vertices[2];

	vec3 v0v1 = v1 - v0;
	vec3 v0v2 = v2 - v0;
	vec3 pvec = cross(ray.dir, pvec);
	float det = dot(v0v1, pvec);
	if (det < FLT_EPSILON) {
		return false;
	}
	float inv = 1.0 / det;

	// u
	vec3 tvec = vec3(ray.from) - v0;
	float u = dot(tvec, pvec) * inv;
	if (u < -FLT_EPSILON || u > 1 + FLT_EPSILON) { return false; }

	// v
	vec3 qvec = cross(tvec, v0v1);
	float v = dot(ray.dir, qvec) * inv;
	if (v < -FLT_EPSILON || u + v > 1 + FLT_EPSILON) { return false; }

	// t
	double t = 1 - u - v;

	intersectionPoint.x = t;
	intersectionPoint.y = u;
	intersectionPoint.z = v;
	return true;
}
