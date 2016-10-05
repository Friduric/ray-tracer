#include "Triangle.h"

using namespace glm;

Triangle::Triangle(vec3 v1, vec3 v2, vec3 v3, vec3 _normal, vec3 _color) :
	vertices{ v1,v2,v3 }, normal(_normal), color(_color) {

}

Triangle::Triangle(vec3 _vertices[3], vec3 _normal, vec3 _color) :
	normal(_normal), color(_color) {
	for (int i = 0; i < 3; ++i) { vertices[i] = _vertices[i]; }
}

bool Triangle::RayIntersection(const Ray & ray, vec3 & intersectionPoint) {
	return false;
}
