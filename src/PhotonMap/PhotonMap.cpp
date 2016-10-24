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
	std::cout << "Building the photon map ..." << std::endl;

	// Calculate max emissivity so that we can normalize photon radiance.
	float maxEmissivity = 0;
	for (const auto * lightSource : scene.emissiveRenderGroups) {
		const auto * material = lightSource->material;
		float rgb = material->GetEmissionColor().r + material->GetEmissionColor().g + material->GetEmissionColor().b;
		float emissivity = lightSource->material->emissivity * rgb;
		maxEmissivity = glm::max<float>(maxEmissivity, emissivity);
	}
	const float INV_MAX_EMISSIVITY = 1.0f / maxEmissivity;

	// Shoot photons from all light sources.
	for (const auto * lightSource : scene.emissiveRenderGroups) {
		for (unsigned int j = 0; j < PHOTONS_PER_LIGHT_SOURCE; ++j) {
			auto * lightPrimitive = lightSource->primitives[rand() % lightSource->primitives.size()];

			// Create a random photon direction from a random light surface position.
			glm::vec3 randomSurfacePosition = lightPrimitive->GetRandomPositionOnSurface();
			glm::vec3 surfaceNormal = lightPrimitive->GetNormal(randomSurfacePosition);
			glm::vec3 randomHemisphereDirection = Utility::Math::CosineWeightedHemisphereSampleDirection(surfaceNormal);
			Ray ray(randomSurfacePosition, randomHemisphereDirection);
			glm::vec3 photonRadiance = glm::dot(ray.direction, surfaceNormal) * lightSource->material->GetEmissionColor();

			// Iterative deepening.
			for (unsigned int k = 0; k < MAX_DEPTH; ++k) {
				float intersectionDistance;
				unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;

				// Shoot photon.
				if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {

					// The photon hit something.
					glm::vec3 intersectionPosition = ray.from + intersectionDistance * ray.direction;
					Primitive * intersectionPrimitive = scene.renderGroups[intersectionRenderGroupIndex].primitives[intersectionPrimitiveIndex];
					Material * intersectionMaterial = scene.renderGroups[intersectionRenderGroupIndex].material;
					glm::vec3 intersectionNormal = intersectionPrimitive->GetNormal(intersectionPosition);
					glm::vec3 rayReflection = Utility::Math::CosineWeightedHemisphereSampleDirection(intersectionNormal);

					// Indirect photon if deeper than 0.
					if (k > 0) {

						indirectPhotons.insert(KDTreeNode(Photon(intersectionPosition, ray.direction, photonRadiance, intersectionPrimitive)));

						// Calculate probability for reflection/absorption and use Russian roulette to decide whether to reflect or not.
						float p = INV_MAX_EMISSIVITY * (photonRadiance.r + photonRadiance.b + photonRadiance.g);
						float rand = std::rand() / (float)RAND_MAX;
						if (rand > p) {
							//	break;
						}
					}
					// Otherwise direct and shadow photons.
					else {
						directPhotons.insert(KDTreeNode(Photon(intersectionPosition, ray.direction, photonRadiance, intersectionPrimitive)));

						// Create a shadow ray.
						Ray shadowRay(intersectionPosition + 0.01f * ray.direction, ray.direction);
						Primitive * shadowPrimitive = intersectionPrimitive;

						// While we hit a surface keep casting and add shadow photons.
						float shadowIntersectionDistance;
						unsigned int shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx;
						while (scene.RayCast(shadowRay, shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx, shadowIntersectionDistance)) {
							shadowPrimitive = scene.renderGroups[shadowIntersectionRenderGroupIdx].primitives[shadowIntersectionPrimitiveIdx];
							glm::vec3 shadowIntersectionPosition = shadowRay.from + shadowIntersectionDistance * shadowRay.direction;
							shadowPhotons.insert(Photon(shadowIntersectionPosition, ray.direction, glm::vec3(0, 0, 0), shadowPrimitive));

							// Update ray position. Direction is the same all the time.
							shadowRay.from = shadowIntersectionPosition + 0.01f * ray.direction;
						}
					}

					photonRadiance = intersectionMaterial->CalculateDiffuseLighting(ray.direction, rayReflection, intersectionNormal, photonRadiance);
					ray.from = intersectionPosition;
					ray.direction = rayReflection;
				}
				else {
					break;
				}
			}
		}
	}

	// Finalize by balancing and optimizing the kd-trees.
	directPhotons.optimize();
	indirectPhotons.optimize();
	shadowPhotons.optimize();

	// Print.
	std::cout << "Photon map was built successfully." << std::endl;
	std::cout << "Total direct photons: " << directPhotons.size() << std::endl;
	std::cout << "Total indirect photons: " << indirectPhotons.size() << std::endl;
	std::cout << "Total shadow photons: " << shadowPhotons.size() << std::endl;
}

void PhotonMap::GetDirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	photonsInRadius.clear();
	directPhotonsReferenceNode.photon.position = pos;
	directPhotons.find_within_range(directPhotonsReferenceNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetIndirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	photonsInRadius.clear();
	indirectPhotonsReferenceNode.photon.position = pos;
	indirectPhotons.find_within_range(indirectPhotonsReferenceNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetShadowPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	photonsInRadius.clear();
	shadowPhotonsReferenceNode.photon.position = pos;
	shadowPhotons.find_within_range(shadowPhotonsReferenceNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

bool PhotonMap::GetClosestDirectPhotonAtPositionWithinRadius(const glm::vec3 & pos, const float radius, Photon & photon) {
	directPhotonsReferenceNode.photon.position = pos;
	auto result = directPhotons.find_nearest(directPhotonsReferenceNode, radius);
	if (result.first != directPhotons.end()) {
		photon = (*result.first).photon;
		return true;
	}
	return false;
}

