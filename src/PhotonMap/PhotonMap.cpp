#include <iostream>
#include <random>
#include "PhotonMap.h"
#include "../Utility/Math.h"

void PhotonMap::CreatePhotonMap(const Scene & scene, const unsigned int photonsPerLightSource,
								const unsigned int maxPhotonsPerNode, const float minSizeOfNodeBox,
								const unsigned int MAX_DEPTH) {

	// Initialize.
	std::uniform_int_distribution<int> integerDistribution;
	std::default_random_engine generator;
	int primIdx;
	std::cout << "Creating the photon map ..." << std::endl;
	Ray ray;

	// For each light source.
	for (unsigned int i = 0; i < scene.emissiveRenderGroups.size(); ++i) {
		const auto & lightSource = *scene.emissiveRenderGroups[i];
		primIdx = rand() % lightSource.primitives.size();
		const auto & prim = *lightSource.primitives[primIdx];
		for (unsigned int j = 0; j < photonsPerLightSource; ++j) {

			// Shoot photon in a random direction.
			glm::vec3 surfPos = prim.GetRandomPositionOnSurface();
			glm::vec3 normal = prim.GetNormal(surfPos);
			glm::vec3 dir = Math::CosineWeightedHemisphereSampleDirection(normal);

			ray.from = surfPos;
			ray.dir = dir;

			unsigned int intersectionRenderGroupIdx;
			unsigned int intersectionPrimitiveIdx;
			float intersectionDistance;

			if (scene.RayCast(ray, intersectionRenderGroupIdx, intersectionPrimitiveIdx, intersectionDistance)) {

				// Save photon in the octree.
				glm::vec3 hitSurfPos = ray.from + intersectionDistance * ray.dir;
				Primitive & prim = *(scene.renderGroups[intersectionRenderGroupIdx].primitives[intersectionPrimitiveIdx]);
				Material* material = scene.renderGroups[intersectionRenderGroupIdx].material;
				glm::vec3 norm = prim.GetNormal(hitSurfPos);
				glm::vec3 refl = glm::reflect(dir, norm);
				glm::vec3 color = material->CalculateDiffuseLighting(-dir, refl, norm, lightSource.material->GetEmissionColor());
				octree.photons.emplace_back(hitSurfPos, dir, color);

				// Trace each photon in scene.
				for (unsigned int k = 0; k < MAX_DEPTH; ++k) {
					ray.dir = refl;
					ray.from = hitSurfPos;
					dir = refl;
					surfPos = hitSurfPos;
					if (scene.RayCast(ray, intersectionRenderGroupIdx, intersectionPrimitiveIdx, intersectionDistance)) {
						// Save photon in the octree
						hitSurfPos = surfPos + intersectionDistance * dir;
						Primitive & otherPrim = *(scene.renderGroups[intersectionRenderGroupIdx].primitives[intersectionPrimitiveIdx]);
						material = scene.renderGroups[intersectionRenderGroupIdx].material;
						norm = otherPrim.GetNormal(hitSurfPos);
						refl = glm::reflect(dir, norm);
						color = material->CalculateDiffuseLighting(dir, refl, norm, color);
						octree.photons.emplace_back(hitSurfPos, dir, color);
					}
					else {
						// Break if we did not hit any more surface.
						break;
					}
				}
			}

		}
	}

	// Set Up the octree.
	octree.SetUpOctree(scene, maxPhotonsPerNode, minSizeOfNodeBox);
}

std::vector<Photon*> & PhotonMap::GetPhotonsAtPosition(const glm::vec3 pos) {
	return octree.GetNodeClosestToPosition(pos)->photons;
}
