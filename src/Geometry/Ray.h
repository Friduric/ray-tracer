#pragma once

#include <glm.hpp>

class Ray {
public:
	glm::vec4 from, to;
	glm::vec3 color;
	Ray(glm::vec4 from = glm::vec4(), glm::vec4 to = glm::vec4(), glm::vec3 color = glm::vec3());
};