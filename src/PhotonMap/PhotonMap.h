#pragma once
#include "Octree.h"

class PhotonMap
{
public:
	
	/// <summary> 
	/// Shoots all photons in the scene and saves them in a Octree.
	/// </summary>
	/// <param name='scene'> The scene. </param>
	/// <param name='PHOTONS_PER_LIGHT_SOURCE'> The amount of photons used per light source. </param>
	/// <param name='MAX_PHOTONS_PER_NODE'> The maximum amount of photons per node. </param>
	/// <param name='MINIMUM_NODE_BOX_DIMENSION'> The minimum width, height and depth of a nodes box size. </param>
	/// <param name='MAX_DEPTH'> The amount of times each photon will bounce at most. </param>
	PhotonMap(const class Scene & scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE,
			  const unsigned int MAX_PHOTONS_PER_NODE,
			  const unsigned int MAX_DEPTH);

	~PhotonMap();

	/// <summary> 
	/// Returns N closest photons located around the given world position.
	/// Photons are stored in closestPhotons.
	/// </summary>
	/// <param name='pos'> The position to search around. </param>
	void PhotonMap::GetNClosestPhotonsInOctreeNodeOfPosition(std::vector<Photon const*> photons, const glm::vec3 & pos,
															 const int N, std::vector<Photon const*> & closestPhotons) const;

	/// <summary> 
	/// Returns all photons located at the given world position.
	/// </summary>
	/// <param name='pos'> The position to search around. </param>
	std::vector<Photon const*> & GetDirectPhotonsInOctreeNodeOfPosition(const glm::vec3 & pos) const;
	std::vector<Photon const*> & GetIndirectPhotonsInOctreeNodeOfPosition(const glm::vec3 & pos) const;
	std::vector<Photon const*> & GetShadowPhotonsInOctreeNodeOfPosition(const glm::vec3 & pos) const;

private:

	/// <summary> All photons in the octree. </summary>
	std::vector<Photon> directPhotons;
	std::vector<Photon> indirectPhotons;
	std::vector<Photon> shadowPhotons;

	// The data structure photons are saved in.
	Octree* octree;

	/// <summary>
	/// Returns a vector with indexes sorted based on value
	/// in vector v.
	/// </summary>
	std::vector<int> PhotonMap::sortIndexes(const std::vector<float>& v) const;
};


