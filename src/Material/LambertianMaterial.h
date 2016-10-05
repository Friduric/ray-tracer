#pragma once
#include "Material.h"

class LambertianMaterial : public Material {
public:
	glm::vec3 CalculateBRDF(glm::vec3& inDirection, glm::vec3& outDirection,
							glm::vec3& normal, glm::vec3 & incomingIntensity, glm::vec3 & surfaceColor);
};
