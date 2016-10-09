#pragma once
#include "../Scene/Scene.h"
#include "Octree.h"

class PhotonMap
{
public:

	/// <summary> 
	/// Shoots all photons in the scene and saves them in a Octree.
	/// </summary>
	/// <param name='scene'> The scene. </param>
	/// <param name='photonsPerLightSource'> The amount of photons used per light source. </param>
	/// <param name='maxPhotonsPerNode'> The maximum amount of photons per node. </param>
	/// <param name='minSizeOfNodeBox'> The minimum width, height and depth of a nodes box size. </param>
	void CreatePhotonMap(const Scene scene,
						 const int photonsPerLightSource = 100000,
						 const int maxPhotonsPerNode = 100,
						 const float minSizeOfNodeBox = 0.5f);

	/// <summary> 
	/// Returns all photons located at the given world position.
	/// </summary>
	/// <param name='pos'> The position to search around. </param>
	std::vector<Photon*> GetPhotonsAtPosition(const glm::vec3 pos);

private:

	// The data structure photons are saved in.
	Octree octree;	
};




