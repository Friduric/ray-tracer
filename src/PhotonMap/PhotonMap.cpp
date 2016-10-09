#include "PhotonMap.h"

void PhotonMap::CreatePhotonMap(const Scene scene, const int photonsPerLightSource, 
								const int maxPhotonsPerNode, const float minSizeOfNodeBox)
{
	// For each light source 
	for (unsigned int i = 0; i < scene.emissiveRenderGroups.size(); ++i) {
		RenderGroup* lightSource = scene.emissiveRenderGroups[i];
		for (unsigned int j = 0; j < photonsPerLightSource; ++j) {
			// Shoot photon in a random direction
			Photon p;
			//  TODO: add more implementation

			// Save photons in the octree
			octree.photons.push_back(p);
		}
	}

	// Setup the octree.
	octree.SetupOctree();
}

std::vector<Photon*> PhotonMap::GetPhotonsAtPosition(const glm::vec3 pos)
{
	//  TODO: correct implementation
	return std::vector<Photon*>();
}
