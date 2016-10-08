#pragma once

#include "Material.h"
#include <random>
#include "../../Utility/Math.h"

class LambertianMaterial : public Material {
public:
	LambertianMaterial(glm::vec3 surfaceColor, glm::vec3 emissionColor = glm::vec3(0, 0, 0));
	bool IsEmissive() const override;
	glm::vec3 GetSurfaceColor() const override;
	glm::vec3 GetEmissionColor() const override;
	glm::vec3 CalculateBRDF(const glm::vec3 & inDirection, const glm::vec3 & outDirection,
							const glm::vec3 & normal, const glm::vec3 & incomingIntensity) const override;
private:
	glm::vec3 emissionColor;
	glm::vec3 surfaceColor;
	float reflectance = 0.50f; // Should not exceed 2 * pi.
};
