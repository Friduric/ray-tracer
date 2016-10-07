#pragma once
#include <glm.hpp>


class Photon {
public:
	/// <summary> The direction the photon came from. </summary>
	glm::vec3 direction;

	/// <summary> The world position of the photon. </summary>
	glm::vec3 position;

	/// <summary> The color of the photon. </summary>
	glm::vec3 color;
};