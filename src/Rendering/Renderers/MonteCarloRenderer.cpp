#include "MonteCarloRenderer.h"

glm::vec3 MonteCarloRenderer::GetPixelColor(const Ray & ray) {

}

MonteCarloRenderer::MonteCarloRenderer(const Scene & _SCENE, const unsigned int _MAX_DEPTH, const unsigned int _BOUNCES_PER_HIT) :
	SCENE(_SCENE), MAX_DEPTH(_MAX_DEPTH), BOUNCES_PER_HIT(_BOUNCES_PER_HIT), Renderer("Monte Carlo Renderer") { }

glm::vec3 MonteCarloRenderer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	return glm::vec3();
}
