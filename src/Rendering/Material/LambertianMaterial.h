#pragma once

#include "Material.h"

class LambertianMaterial : public Material {
public:
	glm::vec3 CalculateBRDF(const glm::vec3 & inDirection, const glm::vec3 & outDirection,
							const glm::vec3 & normal, const glm::vec3 & incomingIntensity,
							const glm::vec3 & surfaceColor) const;
};
