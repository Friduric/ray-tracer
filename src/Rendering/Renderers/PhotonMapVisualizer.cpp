#include "PhotonMapVisualizer.h"

glm::vec3 PhotonMapVisualizer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

PhotonMapVisualizer::PhotonMapVisualizer(Scene & _scene, const unsigned int PHOTONS_PER_LIGHT_SOURCE,
										 const unsigned int MIN_PHOTONS_PER_NODE, const unsigned int MAX_PHOTON_DEPTH) :
	scene(_scene), Renderer("Photon Map Visualizer") {
	photonMap = new PhotonMap(_scene, PHOTONS_PER_LIGHT_SOURCE, MIN_PHOTONS_PER_NODE, MAX_PHOTON_DEPTH);
}

glm::vec3 PhotonMapVisualizer::TraceRay(const Ray & ray, const unsigned int DEPTH) {

	glm::vec3 colorAccumulator(0, 0, 0);

	// Shoot a ray through the scene and sample using the photon map.
	unsigned int intersectionRenderGroupIndex, intersectionPrimitiveIndex;
	float intersectionDistance;
	glm::vec3 intersectionPoint;

	if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance)) {

		// Direct photons.
		intersectionPoint = ray.from + intersectionDistance * ray.direction;
		Octree::OctreeNode* node = photonMap->GetOctreeNodeOfPosition(intersectionPoint);
		const auto & allDirPhotons = node->directPhotons;
		std::vector<Photon const*> closestDirPhotons;
		photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(allDirPhotons, intersectionPoint, 10, closestDirPhotons);
		for (const Photon * dp : closestDirPhotons) {
			colorAccumulator += dp->color;
		}

		// Indirect photons.
		const auto & allIndirPhotons = node->indirectPhotons;
		std::vector<Photon const*> closestIndirPhotons;
		photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(allIndirPhotons, intersectionPoint, 10, closestIndirPhotons);
		for (const Photon * ip : closestIndirPhotons) {
			colorAccumulator += ip->color;
		}
	}

	return colorAccumulator;
}
