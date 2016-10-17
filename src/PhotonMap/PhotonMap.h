#pragma once
#include "Octree.h"

#include "../Utility/KDTree/kdtree.hpp"

class PhotonMap
{
public:

	/// <summary> 
	/// Constructs a photon map by shooting photons into the scene.
	/// The photons are then stored in an octree data structure.
	/// </summary>
	/// <param name='scene'> The scene which we inject photons into. </param>
	/// <param name='PHOTONS_PER_LIGHT_SOURCE'> The amount of photons used per light source. </param>
	/// <param name='MAX_PHOTONS_PER_NODE'> The maximum amount of photons per octree node. </param>
	/// <param name='MINIMUM_NODE_BOX_DIMENSION'> The minimum width, height and depth of a nodes box size. </param>
	/// <param name='MAX_DEPTH'> The number of bounces each photon will make (at most). </param>
	PhotonMap(const class Scene & scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE, const unsigned int MAX_DEPTH);

	/// <summary> 
	/// Node used in kdTree containing a photon
	/// and additional functionality to work with
	/// the implementation of the KDTree.
	/// </summary>
	struct KDTreeNode {
		typedef float value_type;
		Photon photon;
		value_type operator[](unsigned int n) const{
			return photon.position[n];
		}
		float distance(const KDTreeNode &node){
			return glm::length(node.photon.position - photon.position);
		}
	};

	/// <summary> 
	/// Returns direct photons located within a given radius around a given world position.
	/// The photons are added to the vector photonsInRadius.
	/// </summary>
	/// <param name='node'> The node to search around. </param>
	/// <param name='radius'> The radius to search with. </param>
	/// <param name='photonsInRadius'> Found photons are added to this vector. </param>
	void GetDirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius);

	/// <summary> 
	/// Returns indirect photons located within a given radius around a given world position.
	/// The photons are added to the vector photonsInRadius.
	/// </summary>
	/// <param name='node'> The node to search around. </param>
	/// <param name='radius'> The radius to search with. </param>
	/// <param name='photonsInRadius'> Found photons are added to this vector. </param>
	void GetIndirectPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius);

	/// <summary> 
	/// Returns shadow photons located within a given radius around a given world position.
	/// The photons are added to the vector photonsInRadius.
	/// </summary>
	/// <param name='node'> The node to search around. </param>
	/// <param name='radius'> The radius to search with. </param>
	/// <param name='photonsInRadius'> Found photons are added to this vector. </param>
	void GetShadowPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius);

	/// <summary> 
	/// Finds the closest direct photon located within a given radius around a given world position
	/// and sets photon to the found photon. If no photon is found then false is returned, else true.
	/// </summary>
	/// <param name='node'> The node to search around. </param>
	/// <param name='radius'> The radius to search with. </param>
	/// <param name='photonsInRadius'> Found photons are added to this vector. </param>
	bool PhotonMap::GetClosestDirectPhotonAtPositionWithinRadius(const glm::vec3 & pos, const float radius, Photon & photon);

private:
	/// <summary> Node used as reference to keep searching at in the kd tree. </summary>
	PhotonMap::KDTreeNode refNode;

	KDTree::KDTree<3, KDTreeNode> directPhotonsKDTree;
	KDTree::KDTree<3, KDTreeNode> indirectPhotonsKDTree;
	KDTree::KDTree<3, KDTreeNode> shadowPhotonsKDTree;
};


