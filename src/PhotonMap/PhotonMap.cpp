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

	std::vector<Photon> directPhotons;
	std::vector<Photon> indirectPhotons;
	std::vector<Photon> shadowPhotons;
	std::vector<Photon> causticsPhotons;

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
			glm::vec3 randomHemisphereDirection;
			randomHemisphereDirection = Utility::Math::CosineWeightedHemisphereSampleDirection(surfaceNormal);
			Ray ray(randomSurfacePosition + 0.01f*surfaceNormal, randomHemisphereDirection);
			glm::vec3 photonRadiance = glm::dot(ray.direction, surfaceNormal) * lightSource->material->GetEmissionColor();

			// Iterative deepening.
			for (unsigned int k = 0; k < MAX_DEPTH; ++k) {
				float intersectionDistance;
				unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;

				// Shoot photon.
				if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {

					// The photon hit something.
					glm::vec3 intersectionPosition = ray.from + intersectionDistance * ray.direction;
					const RenderGroup & intersectionRenderGroup = scene.renderGroups[intersectionRenderGroupIndex];
					Primitive * intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
					Material * intersectionMaterial = scene.renderGroups[intersectionRenderGroupIndex].material;
					glm::vec3 intersectionNormal = intersectionPrimitive->GetNormal(intersectionPosition);
					glm::vec3 rayReflection = Utility::Math::CosineWeightedHemisphereSampleDirection(intersectionNormal);
					// Indirect photon if deeper than 0.
					if (k > 0) {
						Photon photon = Photon(intersectionPosition, ray.direction, photonRadiance, intersectionPrimitive);
						indirectPhotons.push_back(photon);
						// Calculate probability for reflection/absorption and use Russian roulette to decide whether to reflect or not.
						float p = INV_MAX_EMISSIVITY * (photonRadiance.r + photonRadiance.b + photonRadiance.g);
						float rand = std::rand() / (float)RAND_MAX;
						if (rand > p) {
							break;
						}
					}
					// Otherwise direct and shadow photons.
					else {
						Photon photon = Photon(intersectionPosition, ray.direction, photonRadiance, intersectionPrimitive);
						directPhotons.push_back(photon);
						// Create a shadow ray.
						Ray shadowRay(intersectionPosition + 0.01f * ray.direction, ray.direction);
						Primitive * shadowPrimitive = intersectionPrimitive;

						// While we hit a surface keep casting and add shadow photons.
						float shadowIntersectionDistance;
						unsigned int shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx;
						while (scene.RayCast(shadowRay, shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx, shadowIntersectionDistance)) {
							shadowPrimitive = scene.renderGroups[shadowIntersectionRenderGroupIdx].primitives[shadowIntersectionPrimitiveIdx];
							glm::vec3 shadowIntersectionPosition = shadowRay.from + shadowIntersectionDistance * shadowRay.direction;
							Photon photon = Photon(shadowIntersectionPosition, ray.direction, glm::vec3(0, 0, 0), shadowPrimitive);
							shadowPhotons.push_back(photon);
							// Update ray position. Direction is the same all the time.
							shadowRay.from = shadowIntersectionPosition + 0.01f * ray.direction;
						}
					}
					photonRadiance = intersectionMaterial->CalculateDiffuseLighting(ray.direction, rayReflection, intersectionNormal, photonRadiance);
					ray.from = intersectionPosition + 0.001f*intersectionNormal;
					ray.direction = rayReflection;
				}
				else {
					break;
				}
			}
		}
	}

	// -------------------------------
	// Caustics photons.
	// -------------------------------
	std::vector<const RenderGroup*> transparentObjects;
	for (const RenderGroup & rg : scene.renderGroups) {
		Material* mat = rg.material;
		if (mat->IsTransparent()) {
			transparentObjects.push_back(&rg);
		}
	}
	if (transparentObjects.size() > 0) {
		for (const auto * lightSource : scene.emissiveRenderGroups) {
			for (unsigned int j = 0; j < PHOTONS_PER_LIGHT_SOURCE; ++j) {
				auto * lightPrimitive = lightSource->primitives[rand() % lightSource->primitives.size()];

				// Create a random photon direction from a random light surface position.
				glm::vec3 randomSurfacePosition = lightPrimitive->GetRandomPositionOnSurface();
				glm::vec3 surfaceNormal = lightPrimitive->GetNormal(randomSurfacePosition);
				glm::vec3 randomHemisphereDirection;
				glm::vec3 posOnSurface = transparentObjects[rand() % transparentObjects.size()]->GetRandomPositionOnSurface();
				randomHemisphereDirection = glm::normalize(posOnSurface - randomSurfacePosition);
				Ray ray(randomSurfacePosition + 0.01f*surfaceNormal, randomHemisphereDirection);
				glm::vec3 photonRadiance = glm::dot(ray.direction, surfaceNormal) * lightSource->material->GetEmissionColor();

				// Iterative deepening.
				for (unsigned int k = 0; k < MAX_DEPTH; ++k) {
					float intersectionDistance;
					unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;

					// Shoot photon.
					if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {

						// The photon hit something.
						glm::vec3 intersectionPosition = ray.from + intersectionDistance * ray.direction;
						const RenderGroup & intersectionRenderGroup = scene.renderGroups[intersectionRenderGroupIndex];
						Primitive * intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
						Material * intersectionMaterial = scene.renderGroups[intersectionRenderGroupIndex].material;
						glm::vec3 intersectionNormal = intersectionPrimitive->GetNormal(intersectionPosition);
						glm::vec3 rayReflection = Utility::Math::CosineWeightedHemisphereSampleDirection(intersectionNormal);

						if (intersectionMaterial->IsTransparent()) {

							const float n1 = 1.0f;
							const float n2 = intersectionMaterial->refractiveIndex;
							glm::vec3 offset = intersectionNormal * 0.1f;
							Ray refractedRay(intersectionPosition - offset, glm::refract(ray.direction, intersectionNormal, n1 / n2));
							// Find out if the ray "exits" the render group anywhere.
							bool hit = scene.RenderGroupRayCast(refractedRay, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);
							// float f = 1.0f;
							if (hit) {
								const auto & refractedRayHitPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
								const glm::vec3 refractedIntersectionPoint = refractedRay.from + refractedRay.direction * intersectionDistance;
								const glm::vec3 refractedHitNormal = refractedRayHitPrimitive->GetNormal(refractedIntersectionPoint);

								photonRadiance = intersectionMaterial->CalculateDiffuseLighting(ray.direction, rayReflection, intersectionNormal, photonRadiance);
								ray.from = refractedIntersectionPoint + refractedRay.direction * 0.001f;
								ray.direction = glm::refract(refractedRay.direction, -refractedHitNormal, n2 / n1);

							}
							else {
								// "Flat reflective surface". Stop tracing
								break;
							}
						}
						// We hit a none refractive surface, store caustics photon if its not on depth 0.
						else if (k > 0) {
							Photon photon = Photon(intersectionPosition, ray.direction, photonRadiance, intersectionPrimitive);
							causticsPhotons.push_back(photon);
							break;
						}
						else {
							break;
						}
					}
					else {
						break;
					}
				}
			}
		}
	}

	// Fix strength of photons based on total photons
	for (Photon & photon : directPhotons) {
		photon.color /= (float)directPhotons.size() + (float)indirectPhotons.size();
		directPhotonsKDTree.insert(KDTreeNode(photon)); // This copy of the photons is inefective but the kd tree crashes when using photon pointers atm
	}
	for (Photon & photon : indirectPhotons) {
		photon.color /= (float)directPhotons.size() + (float)indirectPhotons.size();
		indirectPhotonsKDTree.insert(KDTreeNode(photon));
	}
	for (Photon & photon : shadowPhotons) {
		shadowPhotonsKDTree.insert(KDTreeNode(photon));
	}
	float amountOfCausticsPhotonsPerTransparentObject = causticsPhotons.size() / (float)transparentObjects.size();
	for (Photon & photon : causticsPhotons) {
		photon.color /= amountOfCausticsPhotonsPerTransparentObject;
		causticsPhotonsKDTree.insert(KDTreeNode(photon));
	}

	// Finalize by balancing and optimizing the kd-trees.
	directPhotonsKDTree.optimize();
	indirectPhotonsKDTree.optimize();
	shadowPhotonsKDTree.optimize();
	// Print.
	std::cout << "Photon map was built successfully." << std::endl;
	std::cout << "Total direct photons: " << directPhotonsKDTree.size() << std::endl;
	std::cout << "Total indirect photons: " << indirectPhotonsKDTree.size() << std::endl;
	std::cout << "Total shadow photons: " << shadowPhotonsKDTree.size() << std::endl;
	std::cout << "Total caustics photons: " << causticsPhotonsKDTree.size() << std::endl;
}


void PhotonMap::GetDirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	photonsInRadius.clear();
	directPhotonsReferenceNode.photon.position = pos;
	directPhotonsKDTree.find_within_range(directPhotonsReferenceNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetIndirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	photonsInRadius.clear();
	indirectPhotonsReferenceNode.photon.position = pos;
	indirectPhotonsKDTree.find_within_range(indirectPhotonsReferenceNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetShadowPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	photonsInRadius.clear();
	shadowPhotonsReferenceNode.photon.position = pos;
	shadowPhotonsKDTree.find_within_range(shadowPhotonsReferenceNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

void PhotonMap::GetCausticsPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius) {
	photonsInRadius.clear();
	causticsPhotonsReferenceNode.photon.position = pos;
	causticsPhotonsKDTree.find_within_range(causticsPhotonsReferenceNode, radius, std::back_insert_iterator<std::vector<KDTreeNode>>(photonsInRadius));
}

bool PhotonMap::GetClosestDirectPhotonAtPositionWithinRadius(const glm::vec3 & pos, const float radius, Photon & photon) {
	directPhotonsReferenceNode.photon.position = pos;
	auto result = directPhotonsKDTree.find_nearest(directPhotonsReferenceNode, radius);
	if (result.first != directPhotonsKDTree.end()) {
		photon = (*result.first).photon;
		return true;
	}
	return false;
}

