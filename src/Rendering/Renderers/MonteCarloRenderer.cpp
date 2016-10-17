#include "MonteCarloRenderer.h"

#include "../../Utility/Math.h"
#include "../../includes/glm/gtx/norm.hpp"

#include <iostream>

#define __USE_SPECULAR_LIGHTING true

glm::vec3 MonteCarloRenderer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

MonteCarloRenderer::MonteCarloRenderer(Scene & _scene, const unsigned int _MAX_DEPTH) :
	MAX_DEPTH(_MAX_DEPTH), Renderer("Monte Carlo Renderer", _scene) { }

glm::vec3 MonteCarloRenderer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	if (DEPTH == MAX_DEPTH) {
		return glm::vec3(0);
	}

	assert(DEPTH >= 0 && DEPTH < MAX_DEPTH);
	assert(glm::length(ray.direction) > 1.0f - FLT_EPSILON*2.0f && glm::length(ray.direction) < 1.0f + FLT_EPSILON*2.0f);

	// See if our current ray hits anything in the scene.
	float intersectionDistance;
	unsigned int intersectionPrimitiveIndex, intersectionRenderGroupIndex;
	const bool intersectionFound = scene.RayCast(ray, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);

	// If the ray doesn't intersect, simply return (0, 0, 0).
	if (!intersectionFound) {
		return glm::vec3(0);
	}

	// Calculate intersection point.
	const glm::vec3 intersectionPoint = ray.from + ray.direction * intersectionDistance;

	// Retrieve primitive information for the intersected object. 
	auto & intersectionRenderGroup = scene.renderGroups[intersectionRenderGroupIndex];
	const auto & intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];

	// Calculate hit normal.
	const glm::vec3 hitNormal = intersectionPrimitive->GetNormal(intersectionPoint);
	if (glm::dot(-ray.direction, hitNormal) < FLT_EPSILON) {
		return glm::vec3(0); // Back face culling.
	}

	// Retrieve the intersected surface's material.
	const Material * const hitMaterial = intersectionRenderGroup.material;

	// -------------------------------
	// Emissive lighting.
	// -------------------------------
	if (hitMaterial->IsEmissive()) {
		// std::cout << hitMaterial->GetEmissionColor().b;
		float f = 1.0f;
		if (DEPTH >= 1) {
			f *= glm::dot(-ray.direction, hitNormal);
		}
		return f * hitMaterial->GetEmissionColor();
	}

	glm::vec3 colorAccumulator = glm::vec3(0);
	const float rf = 1.0f - hitMaterial->reflectivity;
	const float tf = 1.0f - hitMaterial->transparency;

	// -------------------------------
	// Direct lighting.
	// -------------------------------
	if (rf > FLT_EPSILON && tf > FLT_EPSILON) {
		for (RenderGroup * lightSource : scene.emissiveRenderGroups) {
			// Create a shadow ray.
			const glm::vec3 randomLightSurfacePosition = lightSource->GetRandomPositionOnSurface();
			const glm::vec3 shadowRayDirection = glm::normalize(randomLightSurfacePosition - intersectionPoint);
			if (glm::dot(shadowRayDirection, hitNormal) < FLT_EPSILON) {
				continue;
			}
			const Ray shadowRay(intersectionPoint + hitNormal * 0.0001f, shadowRayDirection);

			// Cast the shadow ray towards the light source.
			unsigned int shadowRayGroupIndex, shadowRayPrimitiveIndex;
			if (scene.RayCast(shadowRay, shadowRayGroupIndex, shadowRayPrimitiveIndex, intersectionDistance)) {
				const auto & renderGroup = scene.renderGroups[shadowRayGroupIndex];			
				if (&renderGroup == lightSource) {
					// We hit the light. Add it's contribution to the color accumulator.
					const Primitive * lightPrimitive = renderGroup.primitives[shadowRayPrimitiveIndex];
					const glm::vec3 lightNormal = lightPrimitive->GetNormal(shadowRay.from + intersectionDistance * shadowRay.direction);
					float lightFactor = glm::dot(-shadowRay.direction, lightNormal);
					if (lightFactor < FLT_EPSILON) {
						continue;
					}

					// Direct diffuse lighting.
					const glm::vec3 radiance = lightFactor * lightSource->material->GetEmissionColor();
					colorAccumulator += rf * tf * hitMaterial->CalculateDiffuseLighting(-shadowRay.direction, -ray.direction, hitNormal, radiance);

#if __USE_SPECULAR_LIGHTING
					// Specular lighting.
					if (hitMaterial->IsSpecular()) {
						colorAccumulator += rf * tf * hitMaterial->CalculateSpecularLighting(-shadowRay.direction, -ray.direction, hitNormal, radiance);
					}
#endif
				}
			}
		}
	}

	colorAccumulator *= (1.0f / glm::max<float>(1.0f, (float)scene.emissiveRenderGroups.size()));

	// -------------------------------
	// Indirect lighting.
	// -------------------------------
	if (rf > FLT_EPSILON && tf > FLT_EPSILON) {
		// Shoot rays and integrate diffuse lighting based on BRDF to compute indirect lighting. 
		const glm::vec3 reflectionDirection = Utility::Math::CosineWeightedHemisphereSampleDirection(hitNormal);
		assert(dot(reflectionDirection, hitNormal) > -FLT_EPSILON);
		const Ray diffuseRay(intersectionPoint, reflectionDirection);
		const auto incomingRadiance = TraceRay(diffuseRay, DEPTH + 1);
		colorAccumulator += hitMaterial->CalculateDiffuseLighting(-diffuseRay.direction, -ray.direction, hitNormal, incomingRadiance);
	}

	colorAccumulator *= rf * tf;

	// -------------------------------
	// Refracted lighting.
	// -------------------------------
	if (hitMaterial->IsTransparent()) {
		const float n1 = 1.0f;
		const float n2 = hitMaterial->refractiveIndex;

		glm::vec3 offset = hitNormal * 0.001f;
		Ray refractedRay(intersectionPoint - offset, glm::refract(ray.direction, hitNormal, n1 / n2));

		// Find out if the ray "exits" the render group anywhere.
		bool hit = scene.RenderGroupRayCast(refractedRay, intersectionRenderGroupIndex, intersectionPrimitiveIndex, intersectionDistance);
		// float f = 1.0f;
		if (hit) {
			const auto & refractedRayHitPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];

			const glm::vec3 refractedIntersectionPoint = refractedRay.from + refractedRay.direction * intersectionDistance;
			const glm::vec3 refractedHitNormal = refractedRayHitPrimitive->GetNormal(intersectionPoint);

			refractedRay.from = refractedIntersectionPoint + refractedHitNormal * 0.001f;
			refractedRay.direction = glm::refract(refractedRay.direction, -refractedHitNormal, n2 / n1);
			// f = glm::max<float>(0.0f, glm::dot(refractedRay.direction, refractedHitNormal));
		}
		else {
			// "Flat surface". Just keep on tracing.
		}

		colorAccumulator += hitMaterial->transparency * TraceRay(refractedRay, DEPTH + 1);
	}

	// -------------------------------
	// Reflective and specular lighting.
	// -------------------------------
	if (hitMaterial->IsReflective()) {
		// Shoot a reflective ray if the material is reflective.
		Ray reflectedRay(intersectionPoint, glm::reflect(ray.direction, hitNormal));
		colorAccumulator += hitMaterial->reflectivity * TraceRay(reflectedRay, DEPTH + 1);
	}

	// Return result.
	return colorAccumulator * 0.95f;
}
