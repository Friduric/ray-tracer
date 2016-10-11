#include <iostream>
#include <random>
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
					photons.push_back(Photon(intersectionPosition, ray.dir, photonRadiance));
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
	octree = new Octree(photons, MAX_PHOTONS_PER_NODE, MAXIMUM_NODE_BOX_DIMENSION, scene.axisAlignedBoundingBox);
}

PhotonMap::~PhotonMap()
{
	delete octree;
}

std::vector<Photon const*> & PhotonMap::GetPhotonsInOctreeNodeOfPosition(const glm::vec3 & pos) const {
	return octree->GetNodeClosestToPosition(pos)->dataPointers;
}
