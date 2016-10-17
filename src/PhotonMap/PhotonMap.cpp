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

PhotonMap::PhotonMap(const Scene & scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE,
					 const unsigned int MIN_PHOTONS_PER_NODE,
					 const float MIN_DIMENSION_SIZE_OF_NODE,
					 const unsigned int MAX_DEPTH) {

	// Initialize.
	std::cout << "Creating the photon map ..." << std::endl;
	Ray ray;

	// For each light source.
	for (unsigned int i = 0; i < scene.emissiveRenderGroups.size(); ++i) {

		// Fetch data about current light source. 
		const auto * lightSource = scene.emissiveRenderGroups[i];
		const auto * prim = lightSource->primitives[rand() % lightSource->primitives.size()];

		for (unsigned int j = 0; j < PHOTONS_PER_LIGHT_SOURCE; ++j) {

			// Shoot photon in a random direction. 
			ray.from = prim->GetRandomPositionOnSurface();
			glm::vec3 normal = prim->GetNormal(ray.from);
			ray.direction = Utility::Math::CosineWeightedHemisphereSampleDirection(normal);
			glm::vec3 photonRadiance = lightSource->material->GetEmissionColor();

			unsigned int intersectionRenderGroupIdx, intersectionPrimitiveIdx;
			float intersectionDistance;
			for (unsigned int k = 0; k < MAX_DEPTH; ++k) {
				if (scene.RayCast(ray, intersectionRenderGroupIdx, intersectionPrimitiveIdx, intersectionDistance)) {
					// Store photon.
					const float intersectionRadianceFactor = glm::dot(ray.direction, normal);
					glm::vec3 intersectionPosition = ray.from + intersectionDistance * ray.direction;
					Primitive * prim = scene.renderGroups[intersectionRenderGroupIdx].primitives[intersectionPrimitiveIdx];
					Material * material = scene.renderGroups[intersectionRenderGroupIdx].material;
					normal = prim->GetNormal(intersectionPosition);
					glm::vec3 rayReflection = Utility::Math::CosineWeightedHemisphereSampleDirection(normal);
					photonRadiance = material->CalculateDiffuseLighting(ray.direction, rayReflection, normal, photonRadiance * intersectionRadianceFactor);
					// Indirect photon if not on first cast
					if (k > 0) {
						KDTreeNode indirectPhotonNode;
						indirectPhotonNode.photon = Photon(intersectionPosition, ray.direction, photonRadiance);
						indirectPhotonsKDTree.insert(indirectPhotonNode);
					}
					else {
						// else direct photon
						KDTreeNode directPhotonNode;
						directPhotonNode.photon = Photon(intersectionPosition, ray.direction, photonRadiance);
						directPhotonsKDTree.insert(directPhotonNode);
						// Add shadow photons
						/*Ray shadowRay;
						glm::vec3 shadowIntersectionPosition = intersectionPosition - 0.01f * prim->GetNormal(intersectionPosition);// Add space from it to not hit itself
						shadowRay.from = shadowIntersectionPosition;
						shadowRay.direction = ray.direction;
						unsigned int shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx;
						float shadowIntersectionDistance;
						Primitive * shadowPrim = prim;
						// While we hit a surface keep casting and add shadow photons
						while (true) {
							if (scene.RayCast(shadowRay, shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx, shadowIntersectionDistance)) {
								shadowPrim = scene.renderGroups[shadowIntersectionRenderGroupIdx].primitives[shadowIntersectionPrimitiveIdx];
								shadowIntersectionPosition = shadowRay.from + shadowIntersectionDistance* shadowRay.direction;
								KDTreeNode shadowPhotonNode;
								shadowPhotonNode.photon = Photon(intersectionPosition, ray.direction, photonRadiance);
								shadowPhotonsKDTree.insert(shadowPhotonNode);
								//shadowPhotons.push_back(Photon(shadowIntersectionPosition, shadowRay.direction, glm::vec3(0, 0, 0)));
							}
							else {
								break;
							}
							// Setup next ray based on previous, direction is same all the time
							shadowRay.from = shadowIntersectionPosition - 0.1f * shadowPrim->GetNormal(shadowIntersectionPosition);
						}*/
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
}

PhotonMap::~PhotonMap() {}

void PhotonMap::GetDirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) const {
	KDTreeNode refNode;
	refNode.photon.position = pos;
	directPhotonsKDTree.find_within_range(refNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetIndirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) const {
	KDTreeNode refNode;
	refNode.photon.position = pos;
	indirectPhotonsKDTree.find_within_range(refNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetShadowPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) const {
	KDTreeNode refNode;
	refNode.photon.position = pos;
	shadowPhotonsKDTree.find_within_range(refNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}


