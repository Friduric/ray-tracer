#include <iostream>
#include <random>
#include <numeric>
#include <algorithm>
#include "../../includes/glm/gtx/norm.hpp"
#include "PhotonMap.h"
#include "../Utility/Math.h"
#include "../Scene/Scene.h"

PhotonMap::PhotonMap(const Scene & scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE,
					 const unsigned int MAX_PHOTONS_PER_NODE,
					 const float MAXIMUM_NODE_BOX_DIMENSION,
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
			ray.dir = Math::CosineWeightedHemisphereSampleDirection(normal);
			glm::vec3 photonRadiance = lightSource->material->GetEmissionColor();

			unsigned int intersectionRenderGroupIdx, intersectionPrimitiveIdx;
			float intersectionDistance;
			for (unsigned int k = 0; k < MAX_DEPTH; ++k) {
				if (scene.RayCast(ray, intersectionRenderGroupIdx, intersectionPrimitiveIdx, intersectionDistance)) {

					// Save photon in the octree.
					glm::vec3 intersectionPosition = ray.from + intersectionDistance * ray.dir;
					Primitive * prim = scene.renderGroups[intersectionRenderGroupIdx].primitives[intersectionPrimitiveIdx];
					Material * material = scene.renderGroups[intersectionRenderGroupIdx].material;
					glm::vec3 intersectionNormal = prim->GetNormal(intersectionPosition);
					glm::vec3 rayReflection = Math::CosineWeightedHemisphereSampleDirection(intersectionNormal);
					photonRadiance = material->CalculateDiffuseLighting(ray.dir, rayReflection, intersectionNormal, photonRadiance);
					// Indirect photon if not on first cast
					if (k > 0) {
						indirectPhotons.push_back(Photon(intersectionPosition, ray.dir, photonRadiance));
					}
					else {
						// else direct photon
						directPhotons.push_back(Photon(intersectionPosition, ray.dir, photonRadiance));
					}

					// TODO: add shadow photons

					ray.from = intersectionPosition;
					ray.dir = rayReflection;
				}
				else {
					break;
				}
			}
		}
	}

	// Create octree
	octree = new Octree(directPhotons, indirectPhotons, shadowPhotons, MAX_PHOTONS_PER_NODE, MAXIMUM_NODE_BOX_DIMENSION, scene.axisAlignedBoundingBox);
}

PhotonMap::~PhotonMap()
{
	delete octree;
}

void PhotonMap::GetNClosestPhotonsInOctreeNodeOfPosition(std::vector<Photon const*> photons, const glm::vec3 & pos,
														  const int N, std::vector<Photon const*> & closestPhotons) const {
	std::vector<float> distances;
	for (Photon const* p : photons) {
		distances.push_back(glm::distance2(p->position, pos));
	}
	int cappedN = std::min((int)photons.size(), N);
	std::vector<int> sortedIdxs = sortIndexes(distances);
	closestPhotons.clear();
	for (int i = 0; i < cappedN; ++i) {
		closestPhotons.push_back(photons[i]);
	}
}

std::vector<Photon const*> & PhotonMap::GetDirectPhotonsInOctreeNodeOfPosition(const glm::vec3 & pos) const {
	return octree->GetNodeClosestToPosition(pos)->directPhotons;
}

std::vector<Photon const*> & PhotonMap::GetIndirectPhotonsInOctreeNodeOfPosition(const glm::vec3 & pos) const {
	return octree->GetNodeClosestToPosition(pos)->indirectPhotons;
}

std::vector<Photon const*> & PhotonMap::GetShadowPhotonsInOctreeNodeOfPosition(const glm::vec3 & pos) const {
	return octree->GetNodeClosestToPosition(pos)->shadowPhotons;
}

std::vector<int> PhotonMap::sortIndexes(const std::vector<float>& v) const
{
	// initialize original index locations
	std::vector<int> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	std::sort(idx.begin(), idx.end(),
			  [&v](size_t i1, size_t i2) {return v[i1] < v[i2]; });
	return idx;
}