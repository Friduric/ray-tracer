#include "PhotonMapVisualizer.h"

#include <iostream>
#include <algorithm>
#include <iostream>

glm::vec3 PhotonMapVisualizer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

PhotonMapVisualizer::PhotonMapVisualizer(Scene & _scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE,
										 const unsigned int MIN_PHOTONS_PER_NODE, const unsigned int MAX_PHOTON_DEPTH) :
	Renderer("Photon Map Visualizer", _scene) {
	photonMap = new PhotonMap(_scene, PHOTONS_PER_LIGHT_SOURCE, MIN_PHOTONS_PER_NODE, MAX_PHOTON_DEPTH);
}

glm::vec3 PhotonMapVisualizer::TraceRay(const Ray & ray, const unsigned int DEPTH) {

	glm::vec3 colorAccumulator(0, 0, 0);

	// Shoot a ray through the scene and sample using the photon map.
	unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;
	float intersectionDistance;

	if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {		
		glm::vec3 intersectionPoint = ray.from + intersectionDistance * ray.direction;
		// Calculate intersection point, normal and points around to sample within.
		// If a side of the node is within the radius then sample from the node adjacent to that side and
		// also check if we're in a corner of the node then sample from the node nearby that corner but not adjacent
		Octree::OctreeNode* node = photonMap->GetOctreeNodeOfPosition(intersectionPoint);
		Octree::OctreeNode* adjacentXNode = NULL;
		Octree::OctreeNode* adjacentYNode = NULL;
		Octree::OctreeNode* adjacentZNode = NULL;
		Octree::OctreeNode* adjacentCornerNode = NULL;
		AABB aabb = node->axisAlignedBoundingBox;
		glm::vec3 corner = aabb.maximum;
		float radius = glm::distance(node->axisAlignedBoundingBox.GetCenter(), corner)*0.5f;
		glm::vec3 radiusCorner(radius, radius, radius);
		// Add direct illumination photons.
		std::vector<Photon const*> allDirPhotons = node->directPhotons;
		// X max side
		glm::vec3 upperCorner = intersectionPoint + radiusCorner;
		glm::vec3 lowerCorner = intersectionPoint - radiusCorner;
		if (upperCorner.x > aabb.maximum.x) {
			
			adjacentXNode = photonMap->GetOctreeNodeOfPosition(glm::vec3(upperCorner.x, intersectionPoint.y, intersectionPoint.z));
		}
		else if (lowerCorner.x < aabb.minimum.x) {
			adjacentXNode = photonMap->GetOctreeNodeOfPosition(glm::vec3(lowerCorner.x, intersectionPoint.y, intersectionPoint.z));
			
		}
		if (adjacentXNode != NULL) {
			allDirPhotons.insert(allDirPhotons.end(), adjacentXNode->directPhotons.begin(), adjacentXNode->directPhotons.end());
		}
		// Y side
		if (upperCorner.y > aabb.maximum.y) {
			adjacentYNode = photonMap->GetOctreeNodeOfPosition(glm::vec3(intersectionPoint.x, upperCorner.y, intersectionPoint.z));
		}
		else if (lowerCorner.y < aabb.minimum.x) {
			adjacentYNode = photonMap->GetOctreeNodeOfPosition(glm::vec3(intersectionPoint.x, lowerCorner.y, intersectionPoint.z));
		}
		if (adjacentYNode != NULL) {
			allDirPhotons.insert(allDirPhotons.end(), adjacentYNode->directPhotons.begin(), adjacentYNode->directPhotons.end());
		}
		// Z side
		if (upperCorner.z > aabb.maximum.z) {
			adjacentZNode = photonMap->GetOctreeNodeOfPosition(glm::vec3(intersectionPoint.x, intersectionPoint.y, upperCorner.z));
		}
		else if (lowerCorner.z < aabb.minimum.z) {
			adjacentZNode = photonMap->GetOctreeNodeOfPosition(glm::vec3(intersectionPoint.x, intersectionPoint.y, lowerCorner.z));
		}
		if (adjacentZNode != NULL) {
			allDirPhotons.insert(allDirPhotons.end(), adjacentZNode->directPhotons.begin(), adjacentZNode->directPhotons.end());
		}
		// Corner
		if (glm::length(upperCorner) > glm::length(aabb.maximum)) {
			adjacentCornerNode = photonMap->GetOctreeNodeOfPosition(upperCorner);
		}
		else if (glm::length(lowerCorner) < glm::length(aabb.minimum)) {
			adjacentCornerNode = photonMap->GetOctreeNodeOfPosition(lowerCorner);
		}
		if (adjacentCornerNode != NULL) {
			allDirPhotons.insert(allDirPhotons.end(), adjacentCornerNode->directPhotons.begin(), adjacentCornerNode->directPhotons.end());
		}

		std::vector<Photon const*> closestDirPhotons;
		//photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(allDirPhotons, intersectionPoint, 1, closestDirPhotons);
		photonMap->GetPhotonsAtPositionWithinRadius(allDirPhotons, intersectionPoint, radius, closestDirPhotons);
		
		if (closestDirPhotons.size() > 0) {
			for (const Photon * dp : closestDirPhotons) {
				
				float distance = glm::distance(intersectionPoint, dp->position);
				float weight = std::max(0.0f, 1.0f - distance / radius);
				colorAccumulator += weight* dp->color;
			}
			colorAccumulator /= radius;
		}
		// Indirect photons.
		/*const auto & allIndirPhotons = node->indirectPhotons;
		std::vector<Photon const*> closestIndirPhotons;
		photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(allIndirPhotons, intersectionPoint, 10, closestIndirPhotons);
		if (closestIndirPhotons.size() > 0) {
			for (const Photon * ip : closestIndirPhotons) {
				colorAccumulator += ip->color;
			}
			colorAccumulator /= closestIndirPhotons.size();
		}*/
	}

	return colorAccumulator;
}
