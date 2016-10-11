#pragma once

#include <glm.hpp>

class AABB {
public:
	glm::vec3 minimum, maximum;
	AABB(glm::vec3 minimum, glm::vec3 maximum);
};