#pragma once

#include "Material.h"

class OrenNayarMaterial : public Material {
public:
	OrenNayarMaterial(glm::vec3 color, float roughness = 0.5f, float emissivity = 0.0f,
					  float reflectivity = 0.98f, float transparency = 0.0f, float refractiveIndex = 1.0f);
	glm::vec3 GetSurfaceColor() const override;
	glm::vec3 CalculateDiffuseLighting(const glm::vec3 & inDirection, const glm::vec3 & outDirection,
									   const glm::vec3 & normal, const glm::vec3 & incomingIntensity) const override;
private:
	glm::vec3 surfaceColor;
	float roughness;
};