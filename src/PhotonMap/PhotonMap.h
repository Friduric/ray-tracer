#pragma once

#include <kdtree.hpp>

#include "Photon.h"

class PhotonMap
{
public:

	/// <summary> 
	/// Constructs a photon map by shooting photons into the scene.
	/// The photons are then stored in a kd-tree.
	/// </summary>
	/// <param name='scene'> The scene which we inject photons into. </param>
	/// <param name='PHOTONS_PER_LIGHT_SOURCE'> The amount of photons used per light source. </param>
	/// <param name='MAX_DEPTH'> The number of bounces each photon will make (at most). </param>
	PhotonMap(const class Scene & scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE, const unsigned int MAX_DEPTH);

	/// <summary> 
	/// A photon (node) wrapper for use with kdtree++.
	/// </summary>
	class KDTreeNode {
	public:
		using value_type = float;
		Photon photon;
		KDTreeNode() {}
		KDTreeNode(const Photon & _photon) : photon(_photon) {}
		value_type operator[](unsigned int n) const { return photon.position[n]; }
		float distance(const KDTreeNode & other) { return glm::distance(other.photon.position, photon.position); }
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
	/// Returns caustics photons located within a given radius around a given world position.
	/// The photons are added to the vector photonsInRadius.
	/// </summary>
	/// <param name='node'> The node to search around. </param>
	/// <param name='radius'> The radius to search with. </param>
	/// <param name='photonsInRadius'> Found photons are added to this vector. </param>
	void GetCausticsPhotonsAtPositionWithinRadius(const glm::vec3 & pos, const float radius, std::vector<KDTreeNode> & photonsInRadius);

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
	PhotonMap::KDTreeNode directPhotonsReferenceNode;
	PhotonMap::KDTreeNode indirectPhotonsReferenceNode;
	PhotonMap::KDTreeNode shadowPhotonsReferenceNode;
	PhotonMap::KDTreeNode causticsPhotonsReferenceNode;

	KDTree::KDTree<3, KDTreeNode> directPhotonsKDTree;
	KDTree::KDTree<3, KDTreeNode> indirectPhotonsKDTree;
	KDTree::KDTree<3, KDTreeNode> shadowPhotonsKDTree;
	KDTree::KDTree<3, KDTreeNode> causticsPhotonsKDTree;
};


