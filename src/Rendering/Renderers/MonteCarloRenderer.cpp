#include "MonteCarloRenderer.h"

#include "../../Utility/Math.h"

#include <iostream>

glm::vec3 MonteCarloRenderer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

MonteCarloRenderer::MonteCarloRenderer(Scene & _scene, const unsigned int _MAX_DEPTH, const unsigned int _BOUNCES_PER_HIT) :
	MAX_DEPTH(_MAX_DEPTH), BOUNCES_PER_HIT(_BOUNCES_PER_HIT), Renderer("Monte Carlo Renderer", _scene) { }

glm::vec3 MonteCarloRenderer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	if (DEPTH == MAX_DEPTH) {
		return glm::vec3(0, 0, 0);
	}

	assert(DEPTH >= 0 && DEPTH < MAX_DEPTH);

	// Disable previous render group if it's convex (we cannot hit it, so no point in doing intersection checks).
	bool reEnableRenderGroup = false;
	if (previousConvexIntersectionRenderGroup != nullptr && previousConvexIntersectionRenderGroup->enabled) {
		reEnableRenderGroup = true; // Always re-enable it. New rays can still hit it.
		previousConvexIntersectionRenderGroup->enabled = false;
	}

	// See if our current ray hits anything in the scene.
	float intersectionDistance;
	unsigned int intersectionPrimitiveIndex, intersectionRenderGroupIndex;
	const bool intersectionFound = scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);
	if (reEnableRenderGroup) {
		previousConvexIntersectionRenderGroup->enabled = true;
	}
	previousConvexIntersectionRenderGroup = nullptr;

	// If the ray doesn't intersect, simply return (0, 0, 0).
	if (!intersectionFound) {
		return glm::vec3(0, 0, 0);
	}

	// Calculate intersection point.
	const glm::vec3 intersectionPoint = ray.from + ray.direction * intersectionDistance;

	// Retrieve primitive information for the intersected object. 
	auto & intersectionRenderGroup = scene.renderGroups[intersectionRenderGroupIndex];
	if (intersectionRenderGroup.convex) {
		previousConvexIntersectionRenderGroup = &intersectionRenderGroup;
	}
	const auto & intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];

	// Calculate normal.
	const glm::vec3 hitNormal = intersectionPrimitive->GetNormal(intersectionPoint);

	// Retrieve the intersected surface's material.
	const Material * const hitMaterial = intersectionRenderGroup.material;
	if (hitMaterial->IsEmissive()) {
		if (DEPTH == 0) {
			return hitMaterial->GetEmissionColor() * 0.0f;
		}
		else {
			return hitMaterial->GetEmissionColor();
		}
	}
	// return glm::vec3(0);

	// Shoot refracted and reflective rays if the material is transparent.
	if (hitMaterial->IsTransparent()) {
		float n1 = 1.0f;
		float n2 = hitMaterial->refractiveIndex;

		glm::vec3 offset = hitNormal * 0.000001f;
		Ray refractedRay(intersectionPoint - offset, glm::refract(ray.direction, hitNormal, n1 / n2));
		Ray reflectedRay(intersectionPoint, glm::reflect(-ray.direction, hitNormal));

		// Find out if the ray "exits" the render group anywhere.
		bool hit = scene.RenderGroupRayCast(refractedRay, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);
		if (hit) {
			const auto & refractedRayHitPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];

			const glm::vec3 refractedIntersectionPoint = refractedRay.from + refractedRay.direction * intersectionDistance;
			const glm::vec3 refractedHitNormal = refractedRayHitPrimitive->GetNormal(intersectionPoint);

			refractedRay.from = refractedIntersectionPoint;
			refractedRay.direction = glm::refract(-refractedRay.direction, refractedHitNormal, n2 / n1);
		}
		else {
			// "Flat surface". Just keep on tracing.
		}

		// Calculate resulting ray contributions.
		float tf = hitMaterial->transparency;
		glm::vec3 refractedRayContribution = TraceRay(refractedRay, DEPTH + 1);
		glm::vec3 reflectedRayContribution = tf < 1.0f ? TraceRay(reflectedRay, DEPTH + 1) : glm::vec3(0);
		return refractedRayContribution * tf + reflectedRayContribution * (1.0f - tf);
	}

	// Shoot rays and integrate based on BRDF. 
	glm::vec3 colorAccumulator = { 0,0,0 };
	for (unsigned int i = 0; i < BOUNCES_PER_HIT; ++i) {
		const glm::vec3 reflectionDirection = Utility::Math::CosineWeightedHemisphereSampleDirection(hitNormal); // glm::reflect(-ray.direction, hitNormal); // 
		assert(dot(reflectionDirection, hitNormal) > -FLT_EPSILON);
		const Ray reflectedRay(intersectionPoint, reflectionDirection);
		const auto incomingRadiance = TraceRay(reflectedRay, DEPTH + 1);
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(-reflectedRay.direction, -ray.direction, hitNormal, incomingRadiance);
		// colorAccumulator += glm::dot(-ray.direction, intersectionPrimitive->GetNormal(intersectionPoint)) * hitMaterial->GetEmissionColor();
	}

	// Return result.
	return (1.0f / (float)BOUNCES_PER_HIT) * colorAccumulator;
}
