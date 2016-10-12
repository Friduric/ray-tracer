#pragma once

#include "Renderer.h"
#include "../../Scene/Scene.h"

class PhotonMapRenderer : public Renderer {
public:
	PhotonMapRenderer(const Scene & scene, const unsigned int MAX_DEPTH = 5, const unsigned int BOUNCES_PER_HIT = 1, 
					  const unsigned int PHOTONS_PER_LIGHT_SOURCE = 1000000, const unsigned int MIN_PHOTONS_PER_NODE = 50, const unsigned int MAX_PHOTON_DEPTH = 3);
	glm::vec3 GetPixelColor(const Ray & ray) override;	
private:
	glm::vec3 TraceRay(const Ray & ray, const unsigned int DEPTH = 0);
	const unsigned int MAX_DEPTH, BOUNCES_PER_HIT;
	const Scene & SCENE;

	glm::vec3 CalculateDirectIlluminationAtPos(const glm::vec3 & pos, const glm::vec3 & incomingDirection,
													  const Primitive & prim, const Material & material) const;
	PhotonMap* photonMap;
};