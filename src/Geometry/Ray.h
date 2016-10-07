#pragma once

#include <glm.hpp>

class Ray {
public:
	glm::vec3 dir, from, to;
	float length;
	Ray(glm::vec3 from, glm::vec3 to);
};