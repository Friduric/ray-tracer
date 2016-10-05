#pragma once

#include <glm.hpp>

class Material {
public:
	/// <summary> Calculates the BRDF value of this material. In other words the outgoing radiance. </summary>
	/// <param name='inDirection'> the direction of the incoming light.</param>
	/// <param name='outDirection'> the outgoing direction towards the eye / another surface.</param>
	/// <param name='normal'> the normal of the surface.</param>
	/// <param name='incomingIntensity'> the intensity of the incoming light.</param>
	/// <param name='surfaceColor'> the color of the surface.</param>
	/// <returns> The BRDF value of this material. </returns>
	virtual glm::vec3 CalculateBRDF(glm::vec3& inDirection, glm::vec3& outDirection, glm::vec3& normal,
									glm::vec3 & incomingIntensity, glm::vec3 & surfaceColor) = 0;
};
