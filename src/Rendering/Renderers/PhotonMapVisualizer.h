#pragma once

#include "Renderer.h"
#include "../../Scene/Scene.h"

class PhotonMapVisualizer : public Renderer {
public:
	glm::vec3 GetPixelColor(const Ray & ray) override;
	PhotonMapVisualizer(Scene & scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE = 1000000, const unsigned int MAX_PHOTON_DEPTH = 3);
private:
	const float PHOTON_SEARCH_RADIUS = 0.05f;
	const float WEIGHT_MODIFIER = 1.3f;
	const float WEIGHT_FACTOR = 1.0f / (WEIGHT_MODIFIER * PHOTON_SEARCH_RADIUS);
	glm::vec3 TraceRay(const Ray & ray, const unsigned int DEPTH = 0);
	PhotonMap * photonMap;
};