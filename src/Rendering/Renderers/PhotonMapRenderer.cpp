#include "PhotonMapRenderer.h"

glm::vec3 PhotonMapRenderer::GetPixelColor(const Ray & ray) {

}

PhotonMapRenderer::PhotonMapRenderer(const Scene & _SCENE, const unsigned int _MAX_DEPTH, const unsigned int _BOUNCES_PER_HIT) :
	SCENE(_SCENE), MAX_DEPTH(_MAX_DEPTH), BOUNCES_PER_HIT(_BOUNCES_PER_HIT), Renderer("Photon Map Renderer") { }

glm::vec3 PhotonMapRenderer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	return glm::vec3();
}
