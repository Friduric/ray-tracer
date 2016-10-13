#pragma once

#include <vector>

#include "Renderer.h"
#include "../../Scene/Scene.h"

class MonteCarloRenderer : public Renderer {
public:
	glm::vec3 GetPixelColor(const Ray & ray) override;
	MonteCarloRenderer(Scene & scene, const unsigned int MAX_DEPTH = 5);
private:
	const unsigned int MAX_DEPTH;

	/// <summary> Traces a ray through the scene. </summary>
	glm::vec3 TraceRay(const Ray & ray, const unsigned int DEPTH = 0);
};