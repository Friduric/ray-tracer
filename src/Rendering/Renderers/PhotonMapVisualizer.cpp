#include "PhotonMapVisualizer.h"

#include <iostream>
#include <algorithm>
#include <iostream>

glm::vec3 PhotonMapVisualizer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

PhotonMapVisualizer::PhotonMapVisualizer(Scene & _scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE,
										 const unsigned int MIN_PHOTONS_PER_NODE,
										 const float MIN_DIMENSION_SIZE_OF_NODE,
										 const unsigned int MAX_PHOTON_DEPTH) :
	Renderer("Photon Map Visualizer", _scene) {
	photonMap = new PhotonMap(_scene, PHOTONS_PER_LIGHT_SOURCE, MIN_PHOTONS_PER_NODE, MIN_DIMENSION_SIZE_OF_NODE, MAX_PHOTON_DEPTH);
}

glm::vec3 PhotonMapVisualizer::TraceRay(const Ray & ray, const unsigned int DEPTH) {

	glm::vec3 colorAccumulator(0.0f, 0.0f, 0.0f);

	// Shoot a ray through the scene and sample using the photon map.
	unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;
	float intersectionDistance;

	if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {
		glm::vec3 intersectionPoint = ray.from + intersectionDistance * ray.direction;
		float radius = 0.3f;
		// Add direct illumination photons.	
		std::vector<PhotonMap::KDTreeNode> directNodes;
		photonMap->GetDirectPhotonsAtPositionWithinRadius(intersectionPoint, radius, directNodes);
		// No photons found return
		if (directNodes.size() == 0) {
			return colorAccumulator;
		}
		for (PhotonMap::KDTreeNode node : directNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance / radius);
			colorAccumulator += weight*node.photon.color;
		}
		

		// Indirect photons.
		std::vector<PhotonMap::KDTreeNode> indirectNodes;
		photonMap->GetIndirectPhotonsAtPositionWithinRadius(intersectionPoint, radius, indirectNodes);
		// No photons found return
		if (indirectNodes.size() == 0) {
			return colorAccumulator;
		}
		for (PhotonMap::KDTreeNode node : indirectNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance / radius);
			colorAccumulator += weight*node.photon.color;
		}
		colorAccumulator /= radius;
	}

	return colorAccumulator;
}

