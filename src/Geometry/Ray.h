#pragma once

#include <glm.hpp>

/// <summary> Describes a 3D ray. </summary>
class Ray {
public:
	glm::vec3 dir, from;
	Ray(glm::vec3 from, glm::vec3 dir);
	Ray();
};