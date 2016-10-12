#include "PhotonMapVisualizer.h"

glm::vec3 PhotonMapVisualizer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

PhotonMapVisualizer::PhotonMapVisualizer(const Scene & _SCENE) :
	SCENE(_SCENE), Renderer("Photon Map Visualizer") { }

glm::vec3 PhotonMapVisualizer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	glm::vec3 colorAccumulator(0,0,0);
	// Shoot a ray through the scene and sample using the photon map.
	unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;
	float intersectionDistance;
	glm::vec3 intersectionPoint;
	if (SCENE.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {
		// direct photons
		intersectionPoint = ray.from + intersectionDistance * ray.direction;
		Octree::OctreeNode* node = SCENE.photonMap->GetOctreeNodeOfPosition(intersectionPoint);
		const auto & allDirPhotons = node->directPhotons;
		std::vector<Photon const*> closestDirPhotons;
		SCENE.photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(allDirPhotons, intersectionPoint, 10, closestDirPhotons);
		for (const Photon * dp : closestDirPhotons) {
			colorAccumulator += dp->color;
		}
		// indirect photons
		const auto & allIndirPhotons = node->indirectPhotons;
		std::vector<Photon const*> closestIndirPhotons;
		SCENE.photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(allIndirPhotons, intersectionPoint, 10, closestIndirPhotons);
		for (const Photon * ip : closestIndirPhotons) {
			colorAccumulator += ip->color;
		}
	}
	return colorAccumulator;
}
