#include "Ray.h"

#include <iostream>

Ray::Ray(glm::vec3 _from, glm::vec3 _dir) :
	from(_from), direction(_dir) {}

Ray::Ray() { }

#if __AABB_TESTING
void Ray::Update() {
	std::cerr << "Ray AABB update has not been implemented yet (Ray.cpp)." << std::endl;
}
#endif