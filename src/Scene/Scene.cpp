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

Primitive & Scene::GetPrimitive(unsigned int renderGroupIndex, unsigned int primitiveIndex) {
	assert(renderGroupIndex < renderGroups.size());
	assert(primitiveIndex < renderGroups[renderGroupIndex].primitives.size());
	return *(renderGroups[renderGroupIndex].primitives[primitiveIndex]);
}

void Scene::RecalculateAABB() {
	glm::vec3 minimum = glm::vec3(FLT_MAX);
	glm::vec3 maximum = glm::vec3(-FLT_MAX);
	for (const auto & rg : renderGroups) {
		const auto & aabb = rg.axisAlignedBoundingBox;
		const auto & min = aabb.minimum;
		const auto & max = aabb.maximum;
		minimum.x = glm::min<float>(min.x, minimum.x);
		minimum.y = glm::min<float>(min.y, minimum.y);
		minimum.z = glm::min<float>(min.z, minimum.z);
		maximum.x = glm::max<float>(max.x, maximum.x);
		maximum.y = glm::max<float>(max.y, maximum.y);
		maximum.z = glm::max<float>(max.z, maximum.z);
	}
	axisAlignedBoundingBox.minimum = minimum;
	axisAlignedBoundingBox.maximum = maximum;
}

void Scene::Initialize() {
	// Pre-store all emissive materials in a separate vector.
	for (unsigned int i = 0; i < renderGroups.size(); ++i) {
		if (renderGroups[i].material->IsEmissive()) {
			emissiveRenderGroups.push_back(&renderGroups[i]);
		}
	}

	RecalculateAABB();
}

bool Scene::RayCast(const Ray & ray, unsigned int & intersectionRenderGroupIndex,
					unsigned int & intersectionPrimitiveIndex, float & intersectionDistance, bool cullBackFace) const {

	float closestInterectionDistance = FLT_MAX;

	// Check if the ray intersects with any enabled render group in the scene.
	// TODO: Use the octree to speed up performance by firstly ray casting on AABBs.
	for (unsigned int i = 0; i < renderGroups.size(); ++i) {
		if (!renderGroups[i].enabled) {
			continue;
		}
		for (unsigned int j = 0; j < renderGroups[i].primitives.size(); ++j) {
			if (!renderGroups[i].primitives[j]->enabled) {
				continue;
			}
			bool intersects = renderGroups[i].primitives[j]->RayIntersection(ray, intersectionDistance);
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

bool Scene::RenderGroupRayCast(const Ray & ray, unsigned int renderGroupIndex, unsigned int & intersectionPrimitiveIndex, float & intersectionDistance, bool cullBackFace) const {
	float closestInterectionDistance = FLT_MAX;

	const auto & renderGroup = renderGroups[renderGroupIndex];

	for (unsigned int j = 0; j < renderGroup.primitives.size(); ++j) {
		if (!renderGroup.primitives[j]->enabled) {
			continue;
		}
		bool intersects = renderGroup.primitives[j]->RayIntersection(ray, intersectionDistance);
		if (intersects) {
			assert(intersectionDistance > FLT_EPSILON);
			if (intersectionDistance < closestInterectionDistance) {
				intersectionPrimitiveIndex = j;
				closestInterectionDistance = intersectionDistance;
			}
		}
	}

	intersectionDistance = closestInterectionDistance;
	return closestInterectionDistance < FLT_MAX - FLT_EPSILON;
}