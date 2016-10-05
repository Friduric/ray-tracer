#pragma once
#include "../Geometry/Ray.h"
#include <vector>
#include <glm.hpp>

class Pixel {
public:
	glm::vec3 color;
	std::vector<Ray*> rays;
	Pixel(glm::vec3 color = glm::vec3());
};