#pragma once

#include <glm.hpp>

class Ray {
public:
	glm::vec3 color, dir, from, to;
	Ray(glm::vec3 from = glm::vec3(), glm::vec3 to = glm::vec3(), glm::vec3 color = glm::vec3());
};