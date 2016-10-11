#pragma once
#include <vector>
#include "Photon.h"
#include "../Geometry/AABB.h"

class Octree {

	class OctreeNode {
	public:
		/// <summary> 
		/// Creates and returns an octree node.
		// Size of node is given by min and max values.
		/// </summary>
		OctreeNode();

		/// <summary> Boundaries of node box. </summary>
		AABB axisAlignedBoundingBox;

		const static unsigned int CHILDREN_PER_NODE = 8;

		/// <summary> Child nodes, null if leaf node. </summary>
		OctreeNode* children[CHILDREN_PER_NODE];

		/// <summary> Pointers to photons in this node, empty if not leaf node. </summary>
		std::vector<Photon const*> dataPointers;

		/// <summary> 
		/// Adds all data types that are inside the aabb of this node.
		/// </summary>
		void AddDataTypesInsideAABB(const std::vector<Photon const*> & photons);


		bool IsLeaf() const;
	};

public:
	/// <summary> 
	/// Sets up the octree with all /DataType/s.
	/// </summary>
	/// <param name='container'> The container with all /DataType/s. </param>
	/// <param name='maxPhotonsPerNode'> The maximum amount of /DataType/ per node. </param>
	/// <param name='maxSizeOfNodeBox'> The maximum width, height and depth of a nodes box size. </param>
	/// <param name='axisAlignedBoundingBox'> The axisAlignedBoundingBox. </param>
	Octree(const std::vector<Photon> & container,
					 const unsigned int maxPhotonsPerNode,
					 const float maxSizeOfNodeBox, const AABB & axisAlignedBoundingBox);

	~Octree();
	/// <summary> The root node. </summary>
	OctreeNode root;

	/// <summary> 
	/// Returns the node closest to the given world position.
	/// </summary>
	/// <param name='pos'> The world position. </param>
	OctreeNode* GetNodeClosestToPosition(const glm::vec3 & pos) const;

private:

	/// <summary> 
	/// Deletes all nodes recursively.
	/// </summary>
	void DeleteRecursive(OctreeNode* node);
};


