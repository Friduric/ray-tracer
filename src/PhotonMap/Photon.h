#pragma once

#include <glm.hpp>

#include "../Geometry/Primitive.h"

class Photon {
public:
	Photon();
	Photon(glm::vec3 position, glm::vec3 direction, glm::vec3 color, Primitive* primitive);

	/// <summary> The direction from where the photon came. </summary>
	glm::vec3 direction;

	/// <summary> The world position of the photon. </summary>
	glm::vec3 position;

	/// <summary> The color of the photon. </summary>
	glm::vec3 color;

	/// <summary> The primitive which the photon is placed on. </summary>
	Primitive* primitive;
};