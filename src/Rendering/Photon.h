#pragma once
#include <glm.hpp>


class Photon {
public:
	// The direction the photon came from.
	glm::vec3 direction;
	// The world position of the photon.
	glm::vec3 position;
	// The color of the photon.
	glm::vec3 color;

};