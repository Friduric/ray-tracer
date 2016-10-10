#pragma once

#include <glm.hpp>

class Ray {
public:
	glm::vec3 dir, from;
	Ray(glm::vec3 from, glm::vec3 dir);
	Ray();
};