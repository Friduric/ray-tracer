#include "PhotonMap.h"

#include <iostream>
#include <random>
#include <numeric>
#include <algorithm>

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
					// Save photon in the octree.
					const float intersectionRadianceFactor = glm::dot(ray.direction, normal);
					glm::vec3 intersectionPosition = ray.from + intersectionDistance * ray.direction;
					Primitive * prim = scene.renderGroups[intersectionRenderGroupIdx].primitives[intersectionPrimitiveIdx];
					Material * material = scene.renderGroups[intersectionRenderGroupIdx].material;
					normal = prim->GetNormal(intersectionPosition);
					glm::vec3 rayReflection = Utility::Math::CosineWeightedHemisphereSampleDirection(normal);
					photonRadiance = material->CalculateDiffuseLighting(ray.direction, rayReflection, normal, photonRadiance * intersectionRadianceFactor);
					// Indirect photon if not on first cast
					if (k > 0) {
						indirectPhotons.push_back(Photon(intersectionPosition, ray.direction, photonRadiance));
					}
					else {
						// else direct photon
						directPhotons.push_back(Photon(intersectionPosition, ray.direction, photonRadiance));
						// Add shadow photons
						Ray shadowRay;
						glm::vec3 shadowIntersectionPosition = intersectionPosition - 0.01f * prim->GetNormal(intersectionPosition);// Add space from it to not hit itself
						shadowRay.from = shadowIntersectionPosition;
						shadowRay.direction = ray.direction;
						unsigned int shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx;
						float shadowIntersectionDistance;
						Primitive * shadowPrim = prim;
						// While we hit a surface keep casting and add shadow photons
						/*while (true) {
							if (scene.RayCast(shadowRay, shadowIntersectionRenderGroupIdx, shadowIntersectionPrimitiveIdx, shadowIntersectionDistance)) {
								shadowPrim = scene.renderGroups[shadowIntersectionRenderGroupIdx].primitives[shadowIntersectionPrimitiveIdx];
								shadowIntersectionPosition = shadowRay.from + shadowIntersectionDistance* shadowRay.direction;
								shadowPhotons.push_back(Photon(shadowIntersectionPosition, shadowRay.direction, glm::vec3(0, 0, 0)));
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

	octree = new Octree(directPhotons, indirectPhotons, shadowPhotons, MIN_PHOTONS_PER_NODE, MIN_DIMENSION_SIZE_OF_NODE, scene.axisAlignedBoundingBox);
}

PhotonMap::~PhotonMap() {
	delete octree;
}

void PhotonMap::GetPhotonsAtPositionWithinRadius(const std::vector<Photon const*> & photons, const glm::vec3 & pos,
															 const float radius, std::vector<Photon const*> & photonsInRadius) const {
	for (Photon const* p : photons) {
		if (glm::distance(p->position, pos) <= radius) {
			photonsInRadius.push_back(p);
		}
	}
}

void PhotonMap::GetNClosestPhotonsOfPosition(const std::vector<Photon const*> & photons, const glm::vec3 & pos,
														 const int N, std::vector<Photon const*> & closestPhotons) const {
	std::vector<float> distances;
	for (Photon const* p : photons) {
		distances.push_back(glm::distance2(p->position, pos));
	}
	int cappedN = std::min((int)photons.size(), N);
	std::vector<int> sortedIdxs = Utility::Math::GetSortedIndices(distances);
	for (int i = 0; i < cappedN; ++i) {
		closestPhotons.push_back(photons[i]);
	}
}

void PhotonMap::AddPhotonsFromAdjacentNodes(std::vector<Photon const*> & adjacentPhotons, Octree::OctreeNode* node,
											const glm::vec3 & intersectionPoint, float radius) const {
	std::vector<Octree::OctreeNode*> adjacentNodes;
	adjacentNodes.push_back(node);
	Octree::OctreeNode* adjacentNode;
	assert(radius < glm::distance(node->axisAlignedBoundingBox.maximum, node->axisAlignedBoundingBox.GetCenter()));
	// Check nodes in all different directions
	std::vector<glm::vec3> directions;
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			for (int z = -1; z <= 1; ++z) {
				adjacentNode = GetOctreeNodeOfPosition(glm::vec3(intersectionPoint.x + x*radius, intersectionPoint.y + y*radius, intersectionPoint.z + z*radius));
				// If the adjacentnode is the same as our main node then check if

				// Make sure we don't add the same node again
				bool alreadyAdded = false;
				for (Octree::OctreeNode* alreadyAddedNode : adjacentNodes) {
					if (alreadyAddedNode == adjacentNode) {
						alreadyAdded = true;
						break;
					}
				}
				if (!alreadyAdded) {
					adjacentNodes.push_back(adjacentNode);
				}
			}
		}
	}

	// Add photons
	for (Octree::OctreeNode* addedNode : adjacentNodes) {
		if (addedNode != node) {
			adjacentPhotons.insert(adjacentPhotons.end(), addedNode->directPhotons.begin(), addedNode->directPhotons.end());
		}
	}
	
}

Octree::OctreeNode * PhotonMap::GetOctreeNodeOfPosition(const glm::vec3 & pos) const {
	return octree->GetNodeClosestToPosition(pos);
}


