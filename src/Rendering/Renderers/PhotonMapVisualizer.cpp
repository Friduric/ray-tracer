#include "PhotonMapVisualizer.h"

#include <iostream>
#include <algorithm>
#include <iostream>

glm::vec3 PhotonMapVisualizer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

PhotonMapVisualizer::PhotonMapVisualizer(Scene & _scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE, const unsigned int MAX_PHOTON_DEPTH) :
	Renderer("Photon Map Visualizer", _scene) {
	photonMap = new PhotonMap(_scene, PHOTONS_PER_LIGHT_SOURCE, MAX_PHOTON_DEPTH);
}

glm::vec3 PhotonMapVisualizer::TraceRay(const Ray & ray, const unsigned int DEPTH) {

	glm::vec3 colorAccumulator(0.0f, 0.0f, 0.0f);

	// Shoot a ray through the scene and sample using the photon map.
	unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;
	float intersectionDistance;

	if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {
		glm::vec3 intersectionPoint = ray.from + intersectionDistance * ray.direction;
		glm::vec3 surfaceNormal = scene.renderGroups[intersectionRenderGroupIndex].primitives[intersectionPrimitiveIndex]->GetNormal(intersectionPoint);

		// Add direct illumination photons.	
		std::vector<PhotonMap::KDTreeNode> directNodes;
		photonMap->GetDirectPhotonsAtPositionWithinRadius(intersectionPoint, PHOTON_SEARCH_RADIUS, directNodes);
		for (PhotonMap::KDTreeNode node : directNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance * WEIGHT_FACTOR);
			auto photonNormal = node.photon.primitive->GetNormal(intersectionPoint);
			colorAccumulator += glm::max(0.0f, glm::dot(photonNormal, surfaceNormal)) * weight * node.photon.color;
		}

		// Indirect photons.
		std::vector<PhotonMap::KDTreeNode> indirectNodes;
		photonMap->GetIndirectPhotonsAtPositionWithinRadius(intersectionPoint, PHOTON_SEARCH_RADIUS, indirectNodes);
		for (PhotonMap::KDTreeNode node : indirectNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance * WEIGHT_FACTOR);
			auto photonNormal = node.photon.primitive->GetNormal(intersectionPoint);
			colorAccumulator += glm::max(0.0f, glm::dot(photonNormal, surfaceNormal)) * weight * node.photon.color;
		}

		// Shadow photons.
		/*std::vector<PhotonMap::KDTreeNode> shadowNodes;
		photonMap->GetShadowPhotonsAtPositionWithinRadius(intersectionPoint, PHOTON_SEARCH_RADIUS, shadowNodes);
		for (PhotonMap::KDTreeNode node : shadowNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance * WEIGHT_FACTOR);
			auto photonNormal = node.photon.primitive->GetNormal(intersectionPoint);
			colorAccumulator += glm::max(0.0f, glm::dot(photonNormal, surfaceNormal)) * weight * node.photon.color;
		}*/

		colorAccumulator /= PHOTON_SEARCH_RADIUS;
	}

	return colorAccumulator;
}

