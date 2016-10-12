#include "Scene.h"

#include <memory>
#include <algorithm>
#include <cassert>
#include <iostream>

#include "../../includes/glm/gtx/norm.hpp"
#include "../../includes/glm/gtx/rotate_vector.hpp"

#include "../Rendering/Materials/LambertianMaterial.h"
#include "../Geometry/Sphere.h"

Scene::Scene() {}

Scene::~Scene() {
	for (auto& rg : renderGroups) {
		for (auto& prim : rg.primitives) {
			delete prim;
		}
	}
	for (auto m : materials) {
		delete m;
	}
	delete photonMap;
}

void Scene::Initialize() {
	/* Add emissive materials. */
	for (unsigned int i = 0; i < renderGroups.size(); ++i) {
		if (renderGroups[i].material->IsEmissive()) {
			emissiveRenderGroups.push_back(&renderGroups[i]);
		}
	}

	/* Calculate the bounding box of the scene. */
	glm::vec3 minimumPosition = glm::vec3(FLT_MAX);
	glm::vec3 maximumPosition = glm::vec3(-FLT_MAX);
	for (const auto & rg : renderGroups) {
		for (const auto & p : rg.primitives) {
			const auto & aabb = p->GetAxisAlignedBoundingBox();
			const auto & min = aabb.minimum;
			const auto & max = aabb.maximum;
			minimumPosition.x = glm::min<float>(min.x, minimumPosition.x);
			minimumPosition.y = glm::min<float>(min.y, minimumPosition.y);
			minimumPosition.z = glm::min<float>(min.z, minimumPosition.z);
			maximumPosition.x = glm::max<float>(max.x, maximumPosition.x);
			maximumPosition.y = glm::max<float>(max.y, maximumPosition.y);
			maximumPosition.z = glm::max<float>(max.z, maximumPosition.z);
		}
	}
	axisAlignedBoundingBox = AABB(minimumPosition, maximumPosition);
}

bool Scene::RayCast(const Ray & ray, unsigned int & intersectionRenderGroupIndex,
					unsigned int & intersectionPrimitiveIndex, float & intersectionDistance, bool cullBackFace) const {
	float closestInterectionDistance = FLT_MAX;
	for (unsigned int i = 0; i < renderGroups.size(); ++i) {
		for (unsigned int j = 0; j < renderGroups[i].primitives.size(); ++j) {
			/* Check if the ray intersects with anything. */
			bool intersects = renderGroups[i].primitives[j]->RayIntersection(ray, intersectionDistance, cullBackFace);
			if (intersects) {
				assert(intersectionDistance > FLT_EPSILON);
				if (intersectionDistance < closestInterectionDistance) {
					intersectionRenderGroupIndex = i;
					intersectionPrimitiveIndex = j;
					closestInterectionDistance = intersectionDistance;
				}
			}
		}
	}
	intersectionDistance = closestInterectionDistance;
	return closestInterectionDistance < FLT_MAX - FLT_EPSILON;
}


bool Scene::RefractionRayCast(const Ray & ray, const unsigned int renderGroupIndex,
							  const glm::vec3 & normal,
							  const glm::vec3 & intersectionPoint,
							  Material const * const materialFrom,
							  Material const * const materialTo) const {
	// See https://en.wikipedia.org/wiki/Schlick%27s_approximation for more information.
	float n1 = materialFrom->refractiveIndex;
	float n2 = materialTo->refractiveIndex;
	float R0 = glm::pow((n1 - n2) / (n1 + n2), 2.0f);
	float alpha = glm::dot(normal, -ray.direction);
	float RO = R0 + (1 - R0) * glm::pow((1 - alpha), 5.0f);

}

void Scene::GeneratePhotonMap(const unsigned int PHOTONS_PER_LIGHT_SOURCE,
							  const unsigned int MAX_PHOTONS_PER_NODE,
							  const unsigned int MAX_DEPTH) {
	photonMap = new PhotonMap(*this, PHOTONS_PER_LIGHT_SOURCE, MAX_PHOTONS_PER_NODE, MAX_DEPTH);
}