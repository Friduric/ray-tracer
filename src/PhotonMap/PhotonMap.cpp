#include "PhotonMap.h"

#include <iostream>
#include <random>
#include <numeric>
#include <algorithm>
#include <iterator>

#include "../../includes/glm/gtx/norm.hpp"

#include "../Utility/Math.h"
#include "../Utility/Other.h"
#include "../Scene/Scene.h"

PhotonMap::PhotonMap(const Scene & scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE, const unsigned int MAX_DEPTH) {

	// Initialize.
	std::cout << "Creating the photon map ..." << std::endl;

	// Shoot photons from all light sources.
	for (const auto * lightSource : scene.emissiveRenderGroups) {
		for (unsigned int j = 0; j < PHOTONS_PER_LIGHT_SOURCE; ++j) {
			const auto * primitive = lightSource->primitives[rand() % lightSource->primitives.size()];

			glm::vec3 randomSurfacePosition = primitive->GetRandomPositionOnSurface();
			glm::vec3 surfaceNormal = primitive->GetNormal(randomSurfacePosition);
			glm::vec3 randomHemisphereDirection = Utility::Math::CosineWeightedHemisphereSampleDirection(surfaceNormal);

			// Shoot a photon in a random direction. 
			Ray ray(randomSurfacePosition, randomHemisphereDirection);
			glm::vec3 photonRadiance = lightSource->material->GetEmissionColor();

			// Iterative deepening.
			for (unsigned int k = 0; k < MAX_DEPTH; ++k) {
				unsigned int intersectionRenderGroupIdx, intersectionPrimitiveIdx;
				float intersectionDistance;
				if (scene.RayCast(ray, intersectionRenderGroupIdx, intersectionPrimitiveIdx, intersectionDistance)) {
					
					// Store photon.
					const float intersectionRadianceFactor = glm::dot(ray.direction, surfaceNormal);
					glm::vec3 intersectionPosition = ray.from + intersectionDistance * ray.direction;
					Primitive * primitive = scene.renderGroups[intersectionRenderGroupIdx].primitives[intersectionPrimitiveIdx];
					Material * material = scene.renderGroups[intersectionRenderGroupIdx].material;
					glm::vec3 intersectionNormal = primitive->GetNormal(intersectionPosition);
					glm::vec3 rayReflection = Utility::Math::CosineWeightedHemisphereSampleDirection(intersectionNormal);
					photonRadiance = material->CalculateDiffuseLighting(ray.direction, rayReflection, intersectionNormal, photonRadiance * intersectionRadianceFactor);
					// Indirect photon if not on first cast
					if (k > 0) {
						KDTreeNode indirectPhotonNode;
						indirectPhotonNode.photon = Photon(intersectionPosition, ray.direction, photonRadiance, primitive);
						indirectPhotonsKDTree.insert(indirectPhotonNode);
					}
					else {
						// else direct photon
						KDTreeNode directPhotonNode;
						directPhotonNode.photon = Photon(intersectionPosition, ray.direction, photonRadiance, primitive);
						directPhotonsKDTree.insert(directPhotonNode);
						// Add shadow photons
						Ray shadowRay;
						glm::vec3 shadowIntersectionPosition = intersectionPosition - 0.01f * primitive->GetNormal(intersectionPosition);// Add space from it to not hit itself
						shadowRay.from = shadowIntersectionPosition;
						shadowRay.direction = ray.direction;
						unsigned int shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx;
						float shadowIntersectionDistance;
						Primitive * shadowPrim = primitive;
						// While we hit a surface keep casting and add shadow photons
						while (true) {
							// disable the previously hit surface so we dont hit it again
							if (scene.RayCast(shadowRay, shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx, shadowIntersectionDistance)) {
								shadowPrim = scene.renderGroups[shadowIntersectionRenderGroupIdx].primitives[shadowIntersectionPrimitiveIdx];
								glm::vec3 shadowNormal = shadowPrim->GetNormal(shadowIntersectionPosition);
								shadowIntersectionPosition = shadowRay.from + shadowIntersectionDistance* shadowRay.direction;
								KDTreeNode shadowPhotonNode;
								//shadowPhotonNode.photon = Photon(shadowIntersectionPosition, ray.direction, glm::vec3(0, 0, 0), shadowPrim);
								//shadowPhotonsKDTree.insert(shadowPhotonNode);
							}
							else {
								break;
							}
							// Enable previously hit surface
							// Setup next ray based on previous, direction is same all the time
							shadowRay.from = shadowIntersectionPosition - 0.01f * shadowPrim->GetNormal(shadowIntersectionPosition);
						}
					}

					ray.from = intersectionPosition;
					ray.direction = rayReflection;
				}
				else {
					break;
				}
			}
		}
	}

	directPhotonsKDTree.optimize();
	indirectPhotonsKDTree.optimize();
	shadowPhotonsKDTree.optimize();
}

// -------------------------------
// KDTree functions.
// -------------------------------

void PhotonMap::GetDirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	refNode.photon.position = pos;
	directPhotonsKDTree.find_within_range(refNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetIndirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	refNode.photon.position = pos;
	indirectPhotonsKDTree.find_within_range(refNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetShadowPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	refNode.photon.position = pos;
	shadowPhotonsKDTree.find_within_range(refNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

bool PhotonMap::GetClosestDirectPhotonAtPositionWithinRadius(const glm::vec3 & pos, const float radius, Photon & photon) {
	refNode.photon.position = pos;
	std::pair<KDTree::KDTree<3, KDTreeNode>::const_iterator, float> result = directPhotonsKDTree.find_nearest(refNode, radius);
	KDTreeNode foundNode = (KDTreeNode)*result.first;
	if (result.first != directPhotonsKDTree.end()) {
		photon = foundNode.photon;
		return true;
	}
	return false;
}

