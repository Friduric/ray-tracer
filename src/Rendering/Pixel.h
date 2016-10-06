#pragma once

#include <vector>

#include <glm.hpp>

#include "../Geometry/Ray.h"

class Pixel {
public:
	glm::vec3 color;
	std::vector<Ray*> rays;
	Pixel(glm::vec3 color = glm::vec3());
};