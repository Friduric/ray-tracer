#pragma once
#include <vector>
#include "Photon.h"
#include "../Scene/Scene.h"

class Octree {
	class OctreeNode
	{
	public:

		/// <summary> Boundaries of node box. </summary>
		float xMin, yMin, zMin = FLT_MAX*0.5f;
		float xMax, yMax, zMax = -FLT_MAX*0.5f;

		const static unsigned int CHILDREN_PER_NODE = 8;

		/// <summary> Child nodes, null if leaf node. </summary>
		OctreeNode* children[CHILDREN_PER_NODE];

		/// <summary> Parent node, null if root node. </summary>
		OctreeNode* parent;

		/// <summary> Pointers to photons in this node, empty if not leaf node. </summary>
		std::vector<Photon*> photons;

		bool IsLeaf() const;
	};

public:
	~Octree();
	/// <summary> The root node. </summary>
	OctreeNode root;

	/// <summary> All photons in the octree. </summary>
	std::vector<Photon> photons;

	/// <summary> 
	/// Sets up the octree with all photons based on 
	/// the information in the octree node.
	/// </summary>
	/// <param name='scene'> The scene. </param>
	/// <param name='maxPhotonsPerNode'> The maximum amount of photons per node. </param>
	/// <param name='maxSizeOfNodeBox'> The maximum width, height and depth of a nodes box size. </param>
	void SetUpOctree(const Scene & scene, const unsigned int maxPhotonsPerNode, const float maxSizeOfNodeBox);

	/// <summary> 
	/// Returns the node closest to the given world position.
	/// </summary>
	/// <param name='pos'> The world position. </param>
	OctreeNode* GetNodeClosestToPosition(glm::vec3 pos);

private:

	/// <summary> 
	/// Set up root node values.
	/// </summary>
	void SetUpRootNode(const Scene & scene);

	/// <summary> 
	/// Creates and returns an octree node.
	// Size of node is given by min and max values.
	/// </summary>
	OctreeNode* CreateNode(const std::vector<Photon*> photons,
						   const float xMin, const float xMax,
						   const float yMin, const float yMax,
						   const float zMin, const float zMax);

	/// <summary> 
	/// Deletes all nodes recursively.
	/// </summary>
	void DeleteRecursive(OctreeNode* node);
};




