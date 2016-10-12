#pragma once

#include <queue>
#include <iostream>
#include "../../includes/glm/gtx/norm.hpp"
#include "Octree.h"

Octree::Octree(const std::vector<Photon> & _directPhotons,
			   const std::vector<Photon> & _indirectPhotons,
			   const std::vector<Photon> & _shadowPhotons,
			   const unsigned int maxPhotonsPerNode,
			   const AABB & aabb) : root(new OctreeNode(aabb)) {
	// Add all photons to the root	

	for (unsigned int n = 0; n < _directPhotons.size(); ++n) {
		root->directPhotons.push_back(&_directPhotons[n]);
	}
	for (unsigned int n = 0; n < _indirectPhotons.size(); ++n) {
		root->indirectPhotons.push_back(&_indirectPhotons[n]);
	}
	for (unsigned int n = 0; n < _shadowPhotons.size(); ++n) {
		root->shadowPhotons.push_back(&_shadowPhotons[n]);
	}

	std::queue<OctreeNode*> nodeQueue;
	nodeQueue.push(root);
	float nodeWidth, nodeHeight, nodeDepth;
	float nodeXHalf, nodeYHalf, nodeZHalf;
	float nodeXMin, nodeYMin, nodeZMin;
	float nodeXMax, nodeYMax, nodeZMax;


	int counter = 0;
	// While there are nodes left in the queue divide into 8 subnodes.
	while (!nodeQueue.empty()) {
		OctreeNode* currentNode = nodeQueue.front();
		nodeQueue.pop();
		nodeWidth = currentNode->axisAlignedBoundingBox.maximum.x - currentNode->axisAlignedBoundingBox.minimum.x;
		nodeHeight = currentNode->axisAlignedBoundingBox.maximum.y - currentNode->axisAlignedBoundingBox.minimum.y;
		nodeDepth = currentNode->axisAlignedBoundingBox.maximum.z - currentNode->axisAlignedBoundingBox.minimum.z;

		// If the amount of photons in the node is larger than/equal to maxPhotonsPerNode,
		// and if all sides of the node box is larger than maxSizeOfNodeBox,
		// then split the node into 8 new nodes.
		unsigned int currentTotalPhotons = currentNode->directPhotons.size() + currentNode->indirectPhotons.size() + currentNode->shadowPhotons.size();
		if (currentTotalPhotons < maxPhotonsPerNode*8) {
			continue;
		}
		nodeXHalf = 0.5f * nodeWidth;
		nodeYHalf = 0.5f * nodeHeight;
		nodeZHalf = 0.5f * nodeDepth;
		unsigned int idxCounter = 0;
		for (unsigned int xIdx = 0; xIdx < 2; ++xIdx) {
			nodeXMin = currentNode->axisAlignedBoundingBox.minimum.x + nodeXHalf*xIdx;
			nodeXMax = nodeXMin + nodeXHalf;
			for (unsigned int yIdx = 0; yIdx < 2; ++yIdx) {
				nodeYMin = currentNode->axisAlignedBoundingBox.minimum.y + nodeYHalf*yIdx;
				nodeYMax = nodeYMin + nodeYHalf;
				for (unsigned int zIdx = 0; zIdx < 2; ++zIdx) {
					nodeZMin = currentNode->axisAlignedBoundingBox.minimum.z + nodeZHalf*zIdx;
					nodeZMax = nodeZMin + nodeZHalf;
					// Add new node									
					OctreeNode* newNode = new OctreeNode();
					newNode->axisAlignedBoundingBox.minimum.x = nodeXMin;
					newNode->axisAlignedBoundingBox.maximum.x = nodeXMax;
					newNode->axisAlignedBoundingBox.minimum.y = nodeYMin;
					newNode->axisAlignedBoundingBox.maximum.y = nodeYMax;
					newNode->axisAlignedBoundingBox.minimum.z = nodeZMin;
					newNode->axisAlignedBoundingBox.maximum.z = nodeZMax;
					newNode->AddDataTypesInsideAABB(currentNode->directPhotons, currentNode->indirectPhotons, currentNode->shadowPhotons);
					currentNode->children[idxCounter] = newNode;
					nodeQueue.push(newNode);
					idxCounter++;
				}
			}
		}
	}
}

Octree::~Octree() {
	DeleteRecursive(root);
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

void Octree::OctreeNode::AddDataTypesInsideAABB(const std::vector<Photon const*> & _directPhotons,
												const std::vector<Photon const*> & _indirectPhotons,
												const std::vector<Photon const*> & _shadowPhotons) {

	// Find and add all photons inside the box of this node.
	// Direct photons
	for (auto dp : _directPhotons) {
		if (axisAlignedBoundingBox.IsPointInsideAABB(dp->position)) {
			directPhotons.push_back(dp);
		}
	}
	// Indirect photons
	for (auto ip : _indirectPhotons) {
		if (axisAlignedBoundingBox.IsPointInsideAABB(ip->position)) {
			indirectPhotons.push_back(ip);
		}
	}
	// Shadow photons
	for (auto sp : _shadowPhotons) {
		if (axisAlignedBoundingBox.IsPointInsideAABB(sp->position)) {
			shadowPhotons.push_back(sp);
		}
	}
}

Octree::OctreeNode* Octree::GetNodeClosestToPosition(const glm::vec3 & pos) const {
	OctreeNode* bestNode = root;
	// Search as long as we're not at a leaf and find the closest node to the given position.
	while (!bestNode->IsLeaf()) {
		float closestDistance = FLT_MAX;
		OctreeNode* closestNode = bestNode->children[0];
		for (unsigned int i = 0; i < OctreeNode::CHILDREN_PER_NODE; ++i) {
			OctreeNode* tmpNode = bestNode->children[i];
			float tmpDist = glm::distance2(tmpNode->axisAlignedBoundingBox.GetCenter(), pos);
			if (tmpDist < closestDistance) {
				closestDistance = tmpDist;
				closestNode = bestNode->children[i];
			}
		}
		bestNode = closestNode;
	}
	return bestNode;
}

Octree::OctreeNode::OctreeNode() {
}

Octree::OctreeNode::OctreeNode(AABB _aabb) : axisAlignedBoundingBox(_aabb) {}

bool Octree::OctreeNode::IsLeaf() const {
	return children[0] == NULL;
}
