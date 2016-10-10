#include "PhotonMap.h"
#include <random>
#include "../Utility/Math.h"
void PhotonMap::CreatePhotonMap(const Scene & scene, const unsigned int photonsPerLightSource,
								const unsigned int maxPhotonsPerNode, const float minSizeOfNodeBox)
{
	std::uniform_int_distribution<int> integerDistribution;
	std::default_random_engine generator;
	int primIdx;
	Ray ray;
	// For each light source 
	for (unsigned int i = 0; i < scene.emissiveRenderGroups.size(); ++i) {
		const auto & lightSource = *scene.emissiveRenderGroups[i];
		primIdx = rand() % lightSource.primitives.size();
		const auto & prim = *lightSource.primitives[primIdx];
		for (unsigned int j = 0; j < photonsPerLightSource; ++j) {	
			// Shoot photon in a random direction
			glm::vec3 surfPos = prim.GetRandomPositionOnSurface();
			glm::vec3 normal = prim.GetNormal(surfPos);
			glm::vec3 dir = Math::CosineWeightedHemisphereSampleDirection(normal);
			ray.from = surfPos;
			ray.dir = dir;
			unsigned int intersectionRenderGroupIdx;
			unsigned int intersectionPrimitiveIdx;
			float intersectionDistance;
			if (scene.RayCast(ray, intersectionRenderGroupIdx, intersectionPrimitiveIdx, intersectionDistance)) {
				// Save photon in the octree
				octree.photons.emplace_back(surfPos + intersectionDistance*dir, dir, lightSource.material->GetEmissionColor());

				// TODO: trace ray in scene
			}
			
		}
	}

	// Set Up the octree.
	octree.SetUpOctree(scene, maxPhotonsPerNode, minSizeOfNodeBox);
}

std::vector<Photon*> PhotonMap::GetPhotonsAtPosition(const glm::vec3 pos)
{
	return octree.GetNodeClosestToPosition(pos)->photons;
}
