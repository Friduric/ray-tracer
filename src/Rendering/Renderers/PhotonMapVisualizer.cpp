#include "PhotonMapVisualizer.h"

#include <algorithm>

#define __VISUALIZE_CAUSTICS true // Whether to visualize the caustics photon map or not.
#define __VISUALIZE_DIRECT true // Whether to visualize the direct photons or not.
#define __VISUALIZE_INDIRECT true // Whether to visualize the indirect photons or not.
#define __VISUALIZE_SHADOW true // Whether to visualize the shadow photons or not.

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
		RenderGroup& renderGroup = scene.renderGroups[intersectionRenderGroupIndex];
		glm::vec3 surfaceNormal = scene.renderGroups[intersectionRenderGroupIndex].primitives[intersectionPrimitiveIndex]->GetNormal(intersectionPoint);
		Material * material = renderGroup.material;

#if __VISUALIZE_DIRECT
		std::vector<PhotonMap::KDTreeNode> directNodes;
		photonMap->GetDirectPhotonsAtPositionWithinRadius(intersectionPoint, PHOTON_SEARCH_RADIUS, directNodes);
		glm::vec3 directColorAccumulator(0.0f);
		for (const auto & node : directNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance * WEIGHT_FACTOR);
			auto photonNormal = node.photon.primitive->GetNormal(intersectionPoint);
			glm::vec3 directPhotonColor = glm::max(0.0f, glm::dot(photonNormal, surfaceNormal)) * weight * node.photon.color;
			directColorAccumulator += directPhotonColor;// material->CalculateDiffuseLighting(node.photon.direction, ray.direction, node.photon.primitive->GetNormal(node.photon.position), directPhotonColor);
		}
		if (directNodes.size() > 0) {
			colorAccumulator += directColorAccumulator;
		}
#endif

#if __VISUALIZE_INDIRECT
		// Indirect photons.
		std::vector<PhotonMap::KDTreeNode> indirectNodes;
		photonMap->GetIndirectPhotonsAtPositionWithinRadius(intersectionPoint, PHOTON_SEARCH_RADIUS, indirectNodes);
		glm::vec3 indirectColorAccumulator(0.0f);
		for (const auto & node : indirectNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance * WEIGHT_FACTOR);
			auto photonNormal = node.photon.primitive->GetNormal(intersectionPoint);
			glm::vec3 indirectPhotonColor = glm::max(0.0f, glm::dot(photonNormal, surfaceNormal)) * weight * node.photon.color;
			indirectColorAccumulator += indirectPhotonColor;// material->CalculateDiffuseLighting(node.photon.direction, ray.direction, node.photon.primitive->GetNormal(node.photon.position), indirectPhotonColor);
		}
		if (indirectNodes.size() > 0) {
			colorAccumulator += indirectColorAccumulator;
		}
#endif

#if __VISUALIZE_CAUSTICS
		// Caustics photons.
		std::vector<PhotonMap::KDTreeNode> causticsNodes;
		photonMap->GetCausticsPhotonsAtPositionWithinRadius(intersectionPoint, PHOTON_SEARCH_RADIUS, causticsNodes);
		glm::vec3 causticsColorAccumulator(0.0f);
		for (const auto & node : causticsNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance * WEIGHT_FACTOR);
			auto photonNormal = node.photon.primitive->GetNormal(intersectionPoint);
			glm::vec3 causticPhotonColor = glm::max(0.0f, glm::dot(photonNormal, surfaceNormal)) * weight * node.photon.color;

			causticsColorAccumulator += causticPhotonColor;// material->CalculateDiffuseLighting(node.photon.direction, ray.direction, node.photon.primitive->GetNormal(node.photon.position), causticPhotonColor);
		}
		if (causticsNodes.size() > 0) {
			colorAccumulator += causticsColorAccumulator;
		}
#endif

#if __VISUALIZE_SHADOW
		// Shadow photons.
		std::vector<PhotonMap::KDTreeNode> shadowNodes;
		photonMap->GetShadowPhotonsAtPositionWithinRadius(intersectionPoint, PHOTON_SEARCH_RADIUS, shadowNodes);
		for (const auto & node : shadowNodes) {
			float distance = glm::distance(intersectionPoint, node.photon.position);
			float weight = std::max(0.0f, 1.0f - distance * WEIGHT_FACTOR);
			auto photonNormal = node.photon.primitive->GetNormal(intersectionPoint);
			colorAccumulator += glm::max(0.0f, glm::dot(photonNormal, surfaceNormal)) * weight * glm::vec3(1.0f, 1.0f, 0.1f);
		}

		colorAccumulator /= PHOTON_SEARCH_RADIUS;
	}
#endif

	return colorAccumulator;
}

