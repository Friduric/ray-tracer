#include "Scene.h"

#include <memory>
#include <algorithm>
#include <cassert>
#include <iostream>

#include "../../includes/glm/gtx/norm.hpp"
#include "../../includes/glm/gtx/rotate_vector.hpp"

#include "../Rendering/Materials/LambertianMaterial.h"
#include "../Geometry/Sphere.h"

using namespace std;

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
}

void Scene::Initialize() {
	for (unsigned int i = 0; i < renderGroups.size(); ++i) {
		if (renderGroups[i].material->IsEmissive()) {
			emissiveRenderGroups.push_back(&renderGroups[i]);
		}
	}
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
	float intersectionRadianceFactor = glm::dot(-ray.dir, intersectionPrimitive->GetNormal(intersectionPoint));
	if (hitMaterial->IsEmissive()) {
		// We could also add emission color to the end result. Returning it here speeds up rendering.
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
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(reflectedRay.dir, -ray.dir, hitNormal, incomingRadiance);
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