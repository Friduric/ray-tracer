#include "MonteCarloRenderer.h"

#include "../../Utility/Math.h"

glm::vec3 MonteCarloRenderer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

MonteCarloRenderer::MonteCarloRenderer(const Scene & _SCENE, const unsigned int _MAX_DEPTH, const unsigned int _BOUNCES_PER_HIT) :
	SCENE(_SCENE), MAX_DEPTH(_MAX_DEPTH), BOUNCES_PER_HIT(_BOUNCES_PER_HIT), Renderer("Monte Carlo Renderer") { }

glm::vec3 MonteCarloRenderer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	if (DEPTH == MAX_DEPTH) { return glm::vec3(0, 0, 0); }

	assert(depth > 0);
	assert(bouncesPerHit > 0);

	unsigned int intersectionPrimitiveIndex, intersectionRenderGroupIndex;
	float intersectionDistance;
	bool intersectionFound = SCENE.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);

	/* If the ray doesn't intersect, simply return (0, 0, 0). */
	if (!intersectionFound) { return glm::vec3(0, 0, 0); }

	/*  Calculate intersection point. */
	glm::vec3 intersectionPoint = ray.from + ray.direction * intersectionDistance;

	/* Retrieve primitive and material information for the intersected object. */
	const auto & intersectionRenderGroup = SCENE.renderGroups[intersectionRenderGroupIndex];
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
	for (unsigned int i = 0; i < BOUNCES_PER_HIT; ++i) {
		glm::vec3 reflectionDirection = Math::RandomHemishpereSampleDirection(hitNormal);
		assert(dot(reflectionDirection, hitNormal) > -FLT_EPSILON);
		Ray reflectedRay(intersectionPoint, reflectionDirection);
		const auto incomingRadiance = TraceRay(reflectedRay, DEPTH + 1);
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(-reflectedRay.direction, -ray.direction, hitNormal, incomingRadiance);
	}
	return (1.0f / (float)BOUNCES_PER_HIT) * colorAccumulator;
}
