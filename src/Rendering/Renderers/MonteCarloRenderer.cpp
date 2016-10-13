#include "MonteCarloRenderer.h"

#include "../../Utility/Math.h"

glm::vec3 MonteCarloRenderer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

MonteCarloRenderer::MonteCarloRenderer(Scene & _scene, const unsigned int _MAX_DEPTH, const unsigned int _BOUNCES_PER_HIT) :
	scene(_scene), MAX_DEPTH(_MAX_DEPTH), BOUNCES_PER_HIT(_BOUNCES_PER_HIT), Renderer("Monte Carlo Renderer") { }

glm::vec3 MonteCarloRenderer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	if (DEPTH == MAX_DEPTH) { return glm::vec3(0, 0, 0); }

	assert(DEPTH > 0);
	assert(BOUNCES_PER_HIT > 0);

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

	// Shoot rays and integrate based on BRDF. 
	glm::vec3 colorAccumulator = { 0,0,0 };
	for (unsigned int i = 0; i < BOUNCES_PER_HIT; ++i) {
		const glm::vec3 reflectionDirection = Math::RandomHemishpereSampleDirection(hitNormal);
		assert(dot(reflectionDirection, hitNormal) > -FLT_EPSILON);
		const Ray reflectedRay(intersectionPoint, reflectionDirection);
		const auto incomingRadiance = TraceRay(reflectedRay, DEPTH + 1);
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(-reflectedRay.direction, -ray.direction, hitNormal, incomingRadiance);
		colorAccumulator += glm::dot(-ray.direction, intersectionPrimitive->GetNormal(intersectionPoint)) * hitMaterial->GetEmissionColor();
	}

	// Return result.
	return (1.0f / (float)BOUNCES_PER_HIT) * colorAccumulator;
}
