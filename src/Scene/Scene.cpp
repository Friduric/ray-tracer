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

glm::vec3 Scene::CalculateDirectIlluminationAtPos(const glm::vec3 & pos, const glm::vec3 & incomingDirection, const Primitive & prim, const Material & material) const {
	glm::vec3 colorAccumulator = { 0,0,0 };
	
	if (photonMap != NULL && false) {
		Octree::OctreeNode* node = photonMap->GetOctreeNodeOfPosition(pos);
		std::vector<Photon const*> directPhotons = node->directPhotons;
		std::vector<Photon const*> indirectPhotons = node->indirectPhotons;
		std::vector<Photon const*> shadowPhotons = node->shadowPhotons;
		// If there are no shadow photons and several directPhotons then we approximate light with photons
		if (shadowPhotons.size() == 0 && directPhotons.size() > photonMap->minPhotonsPerNode) {
			std::vector<Photon const*> photonsWithinRadius;
			glm::vec3 corner = node->axisAlignedBoundingBox.maximum;
			float radius = glm::distance(node->axisAlignedBoundingBox.GetCenter(), corner);
			photonMap->GetPhotonsInOctreeNodeOfPositionWithinRadius(directPhotons, pos, radius, photonsWithinRadius);
			for (Photon const* dp : directPhotons) {
				float distance = glm::distance(pos, dp->position);
				float weight = std::max(0.0f, 1.0f - distance/radius);
				colorAccumulator += weight * dp->color;
			}
		}
		// If there are no direct photons we assume its not lit by any light
		if (directPhotons.size()) {
			return glm::vec3(0, 0, 0);
		}
	}
	Ray ray;
	ray.from = pos + 0.01f * prim.GetNormal(pos); // + 0.01 normal to not cast from inside the object	
	unsigned int intersectionRenderGroupIdx;
	unsigned int intersectionPrimitiveIdx;
	float intersectionDistance;
	// Check all light sources
	for (RenderGroup* lightSource : emissiveRenderGroups) {
		glm::vec3 lightSurfPos = lightSource->GetRandomPositionOnSurface();
		ray.direction = glm::normalize(lightSurfPos - ray.from);
		// Cast ray towards light source
		if (RayCast(ray, intersectionRenderGroupIdx, intersectionPrimitiveIdx, intersectionDistance, false)) {
			// Only add color if we did hit the light source we casted towards
			if (&(renderGroups[intersectionRenderGroupIdx]) == lightSource) {
				Primitive* lightPrim = renderGroups[intersectionRenderGroupIdx].primitives[intersectionPrimitiveIdx];
				glm::vec3 lightNormal = lightPrim->GetNormal(ray.from + intersectionDistance * ray.direction);
				const float intersectionRadianceFactor = glm::dot(-ray.direction, lightNormal);
				colorAccumulator +=  material.CalculateDiffuseLighting(-ray.direction, incomingDirection,
																	   prim.GetNormal(ray.from),
																	   lightSource->material->GetEmissionColor() * intersectionRadianceFactor);
			}
		}
	}

	return (1.0f / (float)emissiveRenderGroups.size()) * colorAccumulator;
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
	glm::vec3 intersectionPoint = ray.from + ray.direction * intersectionDistance;

	/* Retrieve primitive and material information for the intersected object. */
	const auto & intersectionRenderGroup = renderGroups[intersectionRenderGroupIndex];
	const auto & intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
	Material* hitMaterial = intersectionRenderGroup.material;
	if (hitMaterial->IsEmissive()) {
		// We could also add emission color to the end result. Returning it here speeds up rendering.
		const float intersectionRadianceFactor = glm::dot(-ray.direction, intersectionPrimitive->GetNormal(intersectionPoint));
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
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(-reflectedRay.direction, -ray.direction, hitNormal, incomingRadiance);
	}
	return (1.0f / (float)bouncesPerHit) * colorAccumulator;
}

glm::vec3 Scene::TraceRayUsingPhotonMap(const Ray & ray, const unsigned int bouncesPerHit, const unsigned int depth) const {
	unsigned int intersectionPrimitiveIndex, intersectionRenderGroupIndex;
	float intersectionDistance;
	bool intersectionFound = RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);

	/* If the ray doesn't intersect, simply return (0, 0, 0). */
	if (!intersectionFound) { return glm::vec3(0, 0, 0); }

	/*  Calculate intersection point. */
	glm::vec3 intersectionPoint = ray.from + ray.direction * intersectionDistance;

	// Calculate the direct light at the current pos and return
	// if we should not trace this ray any further.
	float r = (float)std::rand() / (float)RAND_MAX;
	if (depth == 0 ||  r > 0.75f) { 
		return CalculateDirectIlluminationAtPos(intersectionPoint, ray.direction,
												*renderGroups[intersectionRenderGroupIndex].primitives[intersectionPrimitiveIndex], 
												*renderGroups[intersectionRenderGroupIndex].material);
	}

	assert(depth > 0);
	assert(bouncesPerHit > 0);

	/* Retrieve primitive and material information for the intersected object. */
	const auto & intersectionRenderGroup = renderGroups[intersectionRenderGroupIndex];
	const auto & intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
	Material* hitMaterial = intersectionRenderGroup.material;
	if (hitMaterial->IsEmissive()) {
		// We could also add emission color to the end result. Returning it here speeds up rendering.
		const float intersectionRadianceFactor = glm::dot(-ray.direction, intersectionPrimitive->GetNormal(intersectionPoint));
		return intersectionRadianceFactor * intersectionRenderGroup.material->GetEmissionColor();
	}

	/* Calculate normal. */
	glm::vec3 hitNormal = intersectionPrimitive->GetNormal(intersectionPoint);

	/* Shoot rays and integrate based on BRDF sampling. */
	glm::vec3 colorAccumulator = { 0,0,0 };
	// Only cast against light sources for direct light

	//for (unsigned int i = 0; i < emissiveRenderGroups.size(); ++i) {
	for (unsigned int i = 0; i < bouncesPerHit; ++i) {
		//glm::vec3 surfPos = emissiveRenderGroups[i]->GetRandomPositionOnSurface();//GetCenter();// 
		glm::vec3 reflectionDirection = Math::RandomHemishpereSampleDirection(hitNormal); //glm::normalize(surfPos - intersectionPoint);// = Math::RandomHemishpereSampleDirection(hitNormal);
		/*std::vector<Photon const*> closestPhotons;
		std::vector<Photon const*> photons = photonMap->GetIndirectPhotonsInOctreeNodeOfPosition(intersectionPoint);
		photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(photons, intersectionPoint, 1, closestPhotons);
		photons = photonMap->GetDirectPhotonsInOctreeNodeOfPosition(intersectionPoint);
		photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(photons, intersectionPoint, 1, closestPhotons);
		for (unsigned int pIdx = 0; pIdx < closestPhotons.size(); ++pIdx) {
			glm::vec3 reflectionDirection = -closestPhotons[pIdx]->direction;
			*/assert(dot(reflectionDirection, hitNormal) > -FLT_EPSILON);
			Ray reflectedRay(intersectionPoint, reflectionDirection);
			const glm::vec3 incomingRadiance = TraceRayUsingPhotonMap(reflectedRay, bouncesPerHit, depth - 1);		
			colorAccumulator += hitMaterial->CalculateDiffuseLighting(-reflectedRay.direction, -ray.direction, hitNormal, incomingRadiance);
		//}	
	}
	// Add indirect light from photons
	/*const auto & allIndirPhotons = photonMap->GetIndirectPhotonsInOctreeNodeOfPosition(intersectionPoint);
	std::vector<Photon const*> closestIndirPhotons;
	photonMap->GetNClosestPhotonsInOctreeNodeOfPosition(allIndirPhotons, intersectionPoint, 10, closestIndirPhotons);
	float photonDistance;
	for (const Photon * ip : closestIndirPhotons) {
		photonDistance = glm::distance(intersectionPoint, ip->position);
	    colorAccumulator +=  0.001f*ip->color / photonDistance;
	}*/
	return (1.0f / (float)bouncesPerHit) *colorAccumulator;
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