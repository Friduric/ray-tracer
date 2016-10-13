#include "MonteCarloRenderer.h"

#include "../../Utility/Math.h"

#include <iostream>

glm::vec3 MonteCarloRenderer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

MonteCarloRenderer::MonteCarloRenderer(Scene & _scene, const unsigned int _MAX_DEPTH) :
	MAX_DEPTH(_MAX_DEPTH), Renderer("Monte Carlo Renderer", _scene) { }

glm::vec3 MonteCarloRenderer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	if (DEPTH == MAX_DEPTH) {
		return glm::vec3(0, 0, 0);
	}

	assert(DEPTH >= 0 && DEPTH < MAX_DEPTH);

	// See if our current ray hits anything in the scene.
	float intersectionDistance;
	unsigned int intersectionPrimitiveIndex, intersectionRenderGroupIndex;
	const bool intersectionFound = scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);

	// If the ray doesn't intersect, simply return (0, 0, 0).
	if (!intersectionFound) {
		return glm::vec3(0, 0, 0);
	}

	// Calculate intersection point.
	const glm::vec3 intersectionPoint = ray.from + ray.direction * intersectionDistance;

	// Retrieve primitive information for the intersected object. 
	auto & intersectionRenderGroup = scene.renderGroups[intersectionRenderGroupIndex];
	const auto & intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];

	// Calculate normal.
	const glm::vec3 hitNormal = intersectionPrimitive->GetNormal(intersectionPoint);

	// Retrieve the intersected surface's material.
	const Material * const hitMaterial = intersectionRenderGroup.material;

	glm::vec3 colorAccumulator = { 0,0,0 };

	const float lights = static_cast<float>(scene.emissiveRenderGroups.size());
	if (DEPTH == 0) {

		// Send a shadow ray toward all light sources to check for light visibility.
		for (RenderGroup * lightSource : scene.emissiveRenderGroups) {
			glm::vec3 randomLightSurfacePosition = lightSource->GetRandomPositionOnSurface();
			Ray shadowRay(intersectionPoint, glm::normalize(randomLightSurfacePosition - intersectionPoint));

			// Cast a shadow ray towards the light source.
			if (scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance, false)) {
				const auto & renderGroup = scene.renderGroups[intersectionRenderGroupIndex];
				if (&renderGroup == lightSource) {

					// We hit the light. Add it's contribution to the color accumulator.
					Primitive * lightPrimitive = renderGroup.primitives[intersectionPrimitiveIndex];
					glm::vec3 lightNormal = lightPrimitive->GetNormal(shadowRay.from + intersectionDistance * shadowRay.direction);
					const float intersectionRadianceFactor = glm::max(0.0f, glm::dot(-shadowRay.direction, lightNormal));
					colorAccumulator += intersectionRadianceFactor * lightSource->material->GetEmissionColor();
				}
			}
		}
	}

	// Calculate direct light contribution.
	colorAccumulator *= (1.0f / lights);

	if (hitMaterial->IsTransparent()) {
		// Shoot a refracted ray if the material is transparent.
		float n1 = 1.0f;
		float n2 = hitMaterial->refractiveIndex;

		glm::vec3 offset = hitNormal * 0.000001f;
		Ray refractedRay(intersectionPoint - offset, glm::refract(ray.direction, hitNormal, n1 / n2));

		// Find out if the ray "exits" the render group anywhere.
		bool hit = scene.RenderGroupRayCast(refractedRay, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);
		if (hit) {
			const auto & refractedRayHitPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];

			const glm::vec3 refractedIntersectionPoint = refractedRay.from + refractedRay.direction * intersectionDistance;
			const glm::vec3 refractedHitNormal = refractedRayHitPrimitive->GetNormal(intersectionPoint);

			refractedRay.from = refractedIntersectionPoint;
			refractedRay.direction = glm::refract(refractedRay.direction, -refractedHitNormal, n2 / n1);
		}
		else {
			// "Flat surface". Just keep on tracing.
		}

		colorAccumulator += hitMaterial->transparency * TraceRay(refractedRay, DEPTH + 1);
	}

	if (hitMaterial->IsReflective()) {
		// Shoot a reflective ray if the material is reflective.
		glm::vec3 offset = hitNormal * 0.000001f;
		Ray reflectedRay(intersectionPoint + offset, glm::reflect(ray.direction, hitNormal));
		const auto incomingRadiance = hitMaterial->reflectivity * TraceRay(reflectedRay, DEPTH + 1);
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(-reflectedRay.direction, -ray.direction, hitNormal, incomingRadiance);
	}

	if (hitMaterial->reflectivity < 1.0f - FLT_EPSILON && hitMaterial->transparency < 1.0f - FLT_EPSILON) {
		float rf = 1.0f - hitMaterial->reflectivity;
		float tf = 1.0f - hitMaterial->transparency;

		// Shoot rays and integrate diffuse lighting based on BRDF to compute indirect lighting. 
		const glm::vec3 reflectionDirection = Utility::Math::CosineWeightedHemisphereSampleDirection(hitNormal);
		assert(dot(reflectionDirection, hitNormal) > -FLT_EPSILON);
		const Ray reflectedRay(intersectionPoint, reflectionDirection);
		const auto incomingRadiance = TraceRay(reflectedRay, DEPTH + 1);
		colorAccumulator += rf * tf * hitMaterial->CalculateDiffuseLighting(-reflectedRay.direction, -ray.direction, hitNormal, incomingRadiance);
	}
	// Add emission.
	colorAccumulator += glm::dot(-ray.direction, intersectionPrimitive->GetNormal(intersectionPoint)) * hitMaterial->GetEmissionColor();

	// Return result.
	return colorAccumulator;
}
