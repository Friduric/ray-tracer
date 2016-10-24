#pragma once

#include "Renderer.h"
#include "../../Scene/Scene.h"

class PhotonMapRenderer : public Renderer {
public:
	PhotonMapRenderer(Scene & scene, const unsigned int MAX_DEPTH = 5, const unsigned int BOUNCES_PER_HIT = 1,
					  const unsigned int PHOTONS_PER_LIGHT_SOURCE = 1000000, const unsigned int MAX_PHOTON_DEPTH = 3);
	glm::vec3 GetPixelColor(const Ray & ray) override;
private:
	const unsigned int MAX_DEPTH, BOUNCES_PER_HIT;
	const float PHOTON_SEARCH_RADIUS = 0.06f;
	const float WEIGHT_MODIFIER = 1.0f;
	const float WEIGHT_FACTOR = 1.0f / (WEIGHT_MODIFIER * PHOTON_SEARCH_RADIUS);

	PhotonMap* photonMap;
	std::vector<PhotonMap::KDTreeNode> volatilePhotonMapNodes;

	/// <summary> Traces a ray through the scene. </summary>
	glm::vec3 TraceRay(const Ray & ray, const unsigned int DEPTH = 0);
};