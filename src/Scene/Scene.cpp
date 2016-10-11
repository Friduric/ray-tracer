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
			maximumPosition.x = glm::min<float>(max.x, maximumPosition.x);
			maximumPosition.y = glm::min<float>(max.y, maximumPosition.y);
			maximumPosition.z = glm::min<float>(max.z, maximumPosition.z);
		}
	}

	axisAlignedBoundingBox = AABB(minimumPosition, maximumPosition);
}

glm::vec3 Scene::TraceRay(const Ray & ray, const unsigned int bouncesPerHit, const unsigned int depth) const {
	if (depth == 0) { return glm::vec3(0, 0, 0); }

	assert(depth > 0);
	assert(bouncesPerHit > 0);

	unsigned int intersectionPrimitiveIndex, intersectionRenderGroupIndex;
	float intersectionDistance;
	bool intersectionFound = RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);

	/* If the ray doesn't intersect, simply return (0, 0, 0). */
	if (!intersectionFound) { return glm::vec3(0, 0, 0); }

	/*  Calculate intersection point. */
	glm::vec3 intersectionPoint = ray.from + ray.dir * intersectionDistance;

	/* Retrieve primitive and material information for the intersected object. */
	const auto & intersectionRenderGroup = renderGroups[intersectionRenderGroupIndex];
	const auto & intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
	Material* hitMaterial = intersectionRenderGroup.material;
	if (hitMaterial->IsEmissive()) {
		// We could also add emission color to the end result. Returning it here speeds up rendering.
		const float intersectionRadianceFactor = glm::dot(-ray.dir, intersectionPrimitive->GetNormal(intersectionPoint));
		return intersectionRadianceFactor * intersectionRenderGroup.material->GetEmissionColor();
	}

	/* Calculate normal. */
	glm::vec3 hitNormal = intersectionPrimitive->GetNormal(intersectionPoint);

	/* Shoot rays and integrate based on BRDF sampling. */
	glm::vec3 colorAccumulator = { 0,0,0 };
	for (unsigned int i = 0; i < bouncesPerHit; ++i) {
		glm::vec3 reflectionDirection = Math::RandomHemishpereSampleDirection(hitNormal);
		assert(dot(reflectionDirection, hitNormal) > -FLT_EPSILON);
		Ray reflectedRay(intersectionPoint, reflectionDirection);
		const auto incomingRadiance = TraceRay(reflectedRay, bouncesPerHit, depth - 1);
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(-reflectedRay.dir, -ray.dir, hitNormal, incomingRadiance);
	}
	return (1.0f / (float)bouncesPerHit) * colorAccumulator;
}

glm::vec3 Scene::TraceRayUsingPhotonMap(const Ray & ray, const unsigned int bouncesPerHit, const unsigned int depth) const {
	if (depth == 0) { return glm::vec3(0, 0, 0); }

	assert(depth > 0);
	assert(bouncesPerHit > 0);

	unsigned int intersectionPrimitiveIndex, intersectionRenderGroupIndex;
	float intersectionDistance;
	bool intersectionFound = RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);

	/* If the ray doesn't intersect, simply return (0, 0, 0). */
	if (!intersectionFound) { return glm::vec3(0, 0, 0); }

	/*  Calculate intersection point. */
	glm::vec3 intersectionPoint = ray.from + ray.dir * intersectionDistance;

	/* Retrieve primitive and material information for the intersected object. */
	const auto & intersectionRenderGroup = renderGroups[intersectionRenderGroupIndex];
	const auto & intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
	Material* hitMaterial = intersectionRenderGroup.material;
	if (hitMaterial->IsEmissive()) {
		// We could also add emission color to the end result. Returning it here speeds up rendering.
		const float intersectionRadianceFactor = glm::dot(-ray.dir, intersectionPrimitive->GetNormal(intersectionPoint));
		return intersectionRadianceFactor * intersectionRenderGroup.material->GetEmissionColor();
	}

	/* Calculate normal. */
	glm::vec3 hitNormal = intersectionPrimitive->GetNormal(intersectionPoint);

	/* Shoot rays and integrate based on BRDF sampling. */
	glm::vec3 colorAccumulator = { 0,0,0 };
	for (unsigned int i = 0; i < bouncesPerHit; ++i) {
		glm::vec3 reflectionDirection = Math::RandomHemishpereSampleDirection(hitNormal);
		assert(dot(reflectionDirection, hitNormal) > -FLT_EPSILON);
		Ray reflectedRay(intersectionPoint, reflectionDirection);
		const auto incomingRadiance = TraceRay(reflectedRay, bouncesPerHit, depth - 1);
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(-reflectedRay.dir, -ray.dir, hitNormal, incomingRadiance);
	}
	return (1.0f / (float)bouncesPerHit) * colorAccumulator;
}

bool Scene::RayCast(const Ray & ray, unsigned int & intersectionRenderGroupIndex,
					unsigned int & intersectionPrimitiveIndex, float & intersectionDistance) const {
	float closestInterectionDistance = FLT_MAX;
	for (unsigned int i = 0; i < renderGroups.size(); ++i) {
		for (unsigned int j = 0; j < renderGroups[i].primitives.size(); ++j) {
			/* Check if the ray intersects with anything. */
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

void Scene::GeneratePhotonMap(const unsigned int PHOTONS_PER_LIGHT_SOURCE,
							  const unsigned int MAX_PHOTONS_PER_NODE,
							  const float MAXIMUM_NODE_BOX_DIMENSION,
							  const unsigned int MAX_DEPTH) {
	photonMap = new PhotonMap(*this, PHOTONS_PER_LIGHT_SOURCE, MAX_PHOTONS_PER_NODE,
							  MAXIMUM_NODE_BOX_DIMENSION, MAX_DEPTH);
}