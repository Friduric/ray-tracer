#pragma once
#include <vector>
#include "Photon.h"

class Octree
{
public:
	/// <summary> The root node. </summary>
	OctreeNode root;

	/// <summary> All photons in the octree. </summary>
	std::vector<Photon> photons;

	/// <summary> 
	/// Sets up the octree with all photons based on 
	/// the information in the octree node.
	/// </summary>
	void SetupOctree();

	/// <summary> 
	/// Returns the node closest to the given world position.
	/// <param name='pos'> The world position. </param>
	/// </summary>
	OctreeNode GetNodeAtPosition(glm::vec3 pos);

};

class OctreeNode
{
public:
	/// <summary> Boundaries of node box. </summary>
	float xMin;
	float xMax;
	float yMin;
	float yMax;

	/// <summary> Child nodes, null if leaf node. </summary>
	OctreeNode* children[8];

	/// <summary> Parent node, null if root node. </summary>
	OctreeNode* parent;

	/// <summary> Pointers to photons in this node, empty if not leaf node. </summary>
	std::vector<Photon*> photons;

};

