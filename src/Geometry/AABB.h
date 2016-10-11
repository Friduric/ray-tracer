#pragma once

#include <glm.hpp>

class AABB {
public:
	glm::vec3 minimum, maximum;
	AABB(const glm::vec3 minimum, const glm::vec3 maximum);
	bool IsPointInsideAABB(const glm::vec3 & position) const;
};