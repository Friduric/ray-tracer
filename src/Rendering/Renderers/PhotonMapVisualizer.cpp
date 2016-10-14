#include "PhotonMapVisualizer.h"

#include <iostream>
#include <algorithm>
#include <iostream>

glm::vec3 PhotonMapVisualizer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

PhotonMapVisualizer::PhotonMapVisualizer(Scene & _scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE,
										 const unsigned int MIN_PHOTONS_PER_NODE, 
										 const float MIN_DIMENSION_SIZE_OF_NODE,
										 const unsigned int MAX_PHOTON_DEPTH) :
	Renderer("Photon Map Visualizer", _scene) {
	photonMap = new PhotonMap(_scene, PHOTONS_PER_LIGHT_SOURCE, MIN_PHOTONS_PER_NODE, MIN_DIMENSION_SIZE_OF_NODE, MAX_PHOTON_DEPTH);
}

glm::vec3 PhotonMapVisualizer::TraceRay(const Ray & ray, const unsigned int DEPTH) {

	glm::vec3 colorAccumulator(0.0f, 0.0f, 0.0f);

	// Shoot a ray through the scene and sample using the photon map.
	unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;
	float intersectionDistance;

	if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {		
		glm::vec3 intersectionPoint = ray.from + intersectionDistance * ray.direction;
		// Add direct illumination photons.
		Octree::OctreeNode* node = photonMap->GetOctreeNodeOfPosition(intersectionPoint);
		std::vector<Photon const*> allDirPhotons = node->directPhotons;
		glm::vec3 corner = node->axisAlignedBoundingBox.maximum;
		float radius = glm::distance(node->axisAlignedBoundingBox.GetCenter(), corner)*0.2f;
		photonMap->AddPhotonsFromAdjacentNodes(allDirPhotons, node, intersectionPoint,  radius);
		// No photons found return
		if (allDirPhotons.size() == 0) {
			return colorAccumulator;
		}
		
		std::vector<Photon const*> closestPhotons;
		//photonMap->GetNClosestPhotonsOfPosition(allDirPhotons, intersectionPoint, 10, closestDirPhotons);
		photonMap->GetPhotonsAtPositionWithinRadius(allDirPhotons, intersectionPoint, radius, closestPhotons);
		//radius = glm::distance(allDirPhotons[allDirPhotons.size()-1]->position, intersectionPoint);
		if (closestPhotons.size() > 0) {
			for (const Photon * dp : closestPhotons) {
				float distance = glm::distance(intersectionPoint, dp->position);
				float weight = std::max(0.0f, 1.0f - distance / radius);
				//std::cout << dp->color.r << " " << dp->color.g << " " << dp->color.b << std::endl;
				colorAccumulator += weight*dp->color;
			}
			//std::cout << radius << std::endl;
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

