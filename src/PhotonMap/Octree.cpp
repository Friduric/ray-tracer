#include "Octree.h"
#include <queue>

Octree::~Octree() {
	DeleteRecursive(&root);
}

void Octree::DeleteRecursive(OctreeNode* node) {
	if (node->IsLeaf()) {
		delete node;
	}
	else {
		for (unsigned int i = 0; i < OctreeNode::CHILDREN_PER_NODE; ++i) {
			DeleteRecursive(node->children[i]);
		}
	}
}

void Octree::SetUpOctree(const Scene & scene, const unsigned int maxPhotonsPerNode, const float minSizeOfNodeBox)
{
	SetUpRootNode(scene);
	std::queue<OctreeNode*> nodeQueue;
	nodeQueue.push(&root);
	float nodeWidth, nodeHeight, nodeDepth;
	float nodeXHalf, nodeYHalf, nodeZHalf;
	float nodeXMin, nodeYMin, nodeZMin;
	float nodeXMax, nodeYMax, nodeZMax;
	OctreeNode* currentNode;
	std::vector<Photon*> cpts; // Current photons
	// While there are nodes left in the queue
	// divide into 8 subnodes
	while (!nodeQueue.empty()) {
		currentNode = nodeQueue.front();
		nodeQueue.pop();
		nodeWidth = currentNode->xMax - currentNode->xMin;
		nodeHeight = currentNode->yMax - currentNode->yMin;
		nodeDepth = currentNode->zMax - currentNode->zMin;
		// If the amount of photons in the node is larger than/equal to maxPhotonsPerNode
		// and if all sides of the node box is larger than minSizeOfNodeBox
		// then split the node into 8 new nodes
		if (currentNode->photons.size() >= maxPhotonsPerNode && minSizeOfNodeBox <= nodeWidth &&
			minSizeOfNodeBox <= nodeHeight && minSizeOfNodeBox <= nodeDepth) {
			nodeXHalf = nodeWidth*0.5f;
			nodeYHalf = nodeHeight*0.5f;
			nodeZHalf = nodeDepth*0.5f;
			unsigned int idxCounter = 0;
			for (unsigned int xIdx = 0; xIdx < 2; ++xIdx) {
				nodeXMin = currentNode->xMin + nodeXHalf*xIdx;
				nodeXMax = nodeXMin + nodeXHalf;
				for (unsigned int yIdx = 0; yIdx < 2; ++yIdx) {
					nodeYMin = currentNode->yMin + nodeYHalf*yIdx;
					nodeYMax = nodeYMin + nodeYHalf;
					for (unsigned int zIdx = 0; zIdx < 2; ++zIdx) {
						nodeZMin = currentNode->zMin + nodeZHalf*zIdx;
						nodeZMax = nodeZMin + nodeZHalf;
						// Add new node
						OctreeNode* newNode = CreateNode(cpts, nodeXMin, nodeXMax, nodeYMin, nodeYMax, nodeZMin, nodeZMax);
						currentNode->children[idxCounter] = newNode;
						newNode->parent = currentNode;
						nodeQueue.push(newNode);
						idxCounter++;
					}
				}
			}
		}
	}
}

void Octree::SetUpRootNode(const Scene & scene) {
	root.xMin = scene.xMin; root.xMax = scene.xMax;
	root.yMin = scene.yMin; root.yMax = scene.yMax;
	root.zMin = scene.zMin; root.zMax = scene.zMax;
	// Add all photons to the root	
	for (unsigned int n = 0; n < photons.size(); ++n) {
		root.photons.push_back(&photons[n]);
	}
}

Octree::OctreeNode* Octree::CreateNode(const std::vector<Photon*> photons,
							  const float xMin, const float xMax,
							  const float yMin, const float yMax,
							  const float zMin, const float zMax) {
	OctreeNode* node = new OctreeNode();
	// Set min and max values.
	node->xMin = xMin; node->xMax = xMax;
	node->yMin = yMin; node->yMax = yMax;
	node->zMin = zMin; node->zMax = zMax;

	// Find and add all photons inside the box of this node.
	glm::vec3 pos;
	for (unsigned int i = 0; i < photons.size(); ++i) {
		pos = photons[i]->position;
		if (pos.x >= xMin && pos.x <= xMax &&
			pos.y >= yMin && pos.y <= yMax &&
			pos.z >= zMin && pos.z <= zMax) {
			node->photons.push_back(photons[i]);
		}
	}
	return node;
}

Octree::OctreeNode* Octree::GetNodeClosestToPosition(glm::vec3 pos)
{
	OctreeNode* bestNode = &root;
	// Search as long as we're not at a leaf
	// and find the closest node to the given position.
	while (!bestNode->IsLeaf()) {
		float closestDistance = FLT_MAX;
		OctreeNode* closestNode = bestNode->children[0];
		OctreeNode* tmpNode;
		glm::vec3 tmpPos;
		float tmpDist;
		for (unsigned int i = 0; i < OctreeNode::CHILDREN_PER_NODE; ++i) {
			tmpNode = bestNode->children[i];
			tmpPos = glm::vec3(tmpNode->xMin + (tmpNode->xMax*0.5f - tmpNode->xMin)*0.5f,
							   tmpNode->yMin + (tmpNode->yMax - tmpNode->yMin)*0.5f,
							   tmpNode->zMin + (tmpNode->zMax - tmpNode->zMin)*0.5f);
			tmpDist = glm::distance(tmpPos, pos);
			if(tmpDist < closestDistance) {
				closestDistance = tmpDist;
				closestNode = bestNode->children[i];
			}
		}
		bestNode = closestNode;
	}
	return bestNode;
}

bool Octree::OctreeNode::IsLeaf() const
{
	if (children[0] == NULL) {
		return true;
	}
	return false;
}
