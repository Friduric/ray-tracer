#pragma once
#include "Octree.h"

class PhotonMap
{
public:
	unsigned int minPhotonsPerNode; // TODO: Add summary.
	unsigned int maxPhotonsPerNode; // TODO: Add summary.

	/// <summary> 
	/// Constructs a photon map by shooting photons into the scene.
	/// The photons are then stored in an octree data structure.
	/// </summary>
	/// <param name='scene'> The scene which we inject photons into. </param>
	/// <param name='PHOTONS_PER_LIGHT_SOURCE'> The amount of photons used per light source. </param>
	/// <param name='MAX_PHOTONS_PER_NODE'> The maximum amount of photons per octree node. </param>
	/// <param name='MINIMUM_NODE_BOX_DIMENSION'> The minimum width, height and depth of a nodes box size. </param>
	/// <param name='MAX_DEPTH'> The number of bounces each photon will make (at most). </param>
	PhotonMap(const class Scene & scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE,
			  const unsigned int MIN_PHOTONS_PER_NODE,
			  const unsigned int MAX_DEPTH);

	~PhotonMap();

	/// <summary> 
	/// Returns photons located within a given radius around a given world position.
	/// The photons are stored in the vector /photons/.
	/// </summary>
	/// <param name='photons'> The photons in the node. </param>
	/// <param name='pos'> The position to search around. </param>
	/// <param name='radius'> The radius to search with. </param>
	/// <param name='photonsInRadius'> Found photons are added to this vector. </param>
	void PhotonMap::GetPhotonsInOctreeNodeOfPositionWithinRadius(const std::vector<Photon const*> & photons, const glm::vec3 & pos,
																 const float radius, std::vector<Photon const*> & photonsInRadius) const;

	/// <summary> 
	/// Returns N closest photons located around a given world position.
	/// The photons are stored in the vector /photons/.
	/// </summary>
	/// <param name='photons'> The photons in the node. </param>
	/// <param name='pos'> The position to search around. </param>
	/// <param name='N'> The amount of photons to retrieve. </param>
	/// <param name='closestPhotons'> Found photons are added to this vector. </param>
	void PhotonMap::GetNClosestPhotonsInOctreeNodeOfPosition(const std::vector<Photon const*> & photons, const glm::vec3 & pos,
															 const int N, std::vector<Photon const*> & closestPhotons) const;

	/// <summary> 
	/// Returns the octree node which contains a given world position.
	/// </summary>
	/// <param name='pos'> The position to search around. </param>
	Octree::OctreeNode * PhotonMap::GetOctreeNodeOfPosition(const glm::vec3 & pos) const;

private:

	/// <summary> Container for all the direct photons. </summary>
	std::vector<Photon> directPhotons;

	/// <summary> Container for all the indirect photons. </summary>
	std::vector<Photon> indirectPhotons;

	/// <summary> Container for all the shadow photons. </summary>
	std::vector<Photon> shadowPhotons;

	/// <summary> An octree used for quick fetching of photons. </summary>
	Octree * octree;
};


