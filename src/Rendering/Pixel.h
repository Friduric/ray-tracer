#pragma once

#include <glm.hpp>

#include "../Geometry/Ray.h"

class Pixel {
public:
	glm::vec3 color;
	Pixel(glm::vec3 color = glm::vec3());
};