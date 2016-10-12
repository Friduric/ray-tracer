#include "PhotonMapVisualizer.h"

glm::vec3 PhotonMapVisualizer::GetPixelColor(const Ray & ray) {

}

PhotonMapVisualizer::PhotonMapVisualizer(const Scene & _SCENE) :
	SCENE(_SCENE), Renderer("Photon Map Visualizer") { }

glm::vec3 PhotonMapVisualizer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	return glm::vec3();
}
