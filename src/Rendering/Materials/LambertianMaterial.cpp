#include "LambertianMaterial.h"

LambertianMaterial::LambertianMaterial(glm::vec3 color, float _emissivity, float _reflectivity,
									   float _transparency, float _refractiveIndex) :
	surfaceColor(color), Material(_emissivity, _reflectivity, _transparency, _refractiveIndex, 0.0f) {}

glm::vec3 LambertianMaterial::GetSurfaceColor() const { return surfaceColor; }

glm::vec3 LambertianMaterial::CalculateDiffuseLighting(const glm::vec3 & inDirection,
													   const glm::vec3 & outDirection,
													   const glm::vec3 & normal,
													   const glm::vec3 & incomingRadiance) const {
	return glm::max(0.0f, glm::dot(-inDirection, normal)) * (incomingRadiance * surfaceColor);
}


