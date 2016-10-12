#pragma once

#include "Renderer.h"
#include "../../Scene/Scene.h"

class MonteCarloRenderer : public Renderer {
public:
	glm::vec3 GetPixelColor(const Ray & ray) override;
	MonteCarloRenderer(const Scene & scene, const unsigned int MAX_DEPTH = 5, const unsigned int BOUNCES_PER_HIT = 1);
private:
	glm::vec3 TraceRay(const Ray & ray, const unsigned int DEPTH = 0);
	const unsigned int MAX_DEPTH, BOUNCES_PER_HIT;
	const Scene & SCENE;
};