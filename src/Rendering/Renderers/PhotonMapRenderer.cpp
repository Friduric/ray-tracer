#include "PhotonMapRenderer.h"

#include <algorithm>

glm::vec3 PhotonMapRenderer::GetPixelColor(const Ray & ray) {
	return TraceRay(ray);
}

PhotonMapRenderer::PhotonMapRenderer(const Scene & _SCENE, const unsigned int _MAX_DEPTH, const unsigned int _BOUNCES_PER_HIT) :
	SCENE(_SCENE), MAX_DEPTH(_MAX_DEPTH), BOUNCES_PER_HIT(_BOUNCES_PER_HIT), Renderer("Photon Map Renderer") { }

glm::vec3 PhotonMapRenderer::TraceRay(const Ray & ray, const unsigned int DEPTH) {
	return glm::vec3();
}

glm::vec3 PhotonMapRenderer::CalculateDirectIlluminationAtPos(const glm::vec3 & pos, const glm::vec3 & incomingDirection, const Primitive & prim, const Material & material) const {
	glm::vec3 colorAccumulator = { 0,0,0 };

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
			float weight = std::max(0.0f, 1.0f - distance / radius);
			colorAccumulator += weight * dp->color;
		}
	}
	// If there are no direct photons we assume its not lit by any light
	if (directPhotons.size()) {
		return glm::vec3(0, 0, 0);
	}

	Ray ray;
	ray.from = pos + 0.01f * prim.GetNormal(pos); // + 0.01 normal to not cast from inside the object	
	unsigned int intersectionRenderGroupIdx;
	unsigned int intersectionPrimitiveIdx;
	float intersectionDistance;
	// Check all light sources
	for (RenderGroup* lightSource : SCENE.emissiveRenderGroups) {
		glm::vec3 lightSurfPos = lightSource->GetRandomPositionOnSurface();
		ray.direction = glm::normalize(lightSurfPos - ray.from);
		// Cast ray towards light source
		if (SCENE.RayCast(ray, intersectionRenderGroupIdx, intersectionPrimitiveIdx, intersectionDistance, false)) {
			// Only add color if we did hit the light source we casted towards
			const auto & renderGroup = SCENE.renderGroups[intersectionRenderGroupIdx];
			if (&renderGroup == lightSource) {
				Primitive* lightPrim = renderGroup.primitives[intersectionPrimitiveIdx];
				glm::vec3 lightNormal = lightPrim->GetNormal(ray.from + intersectionDistance * ray.direction);
				const float intersectionRadianceFactor = glm::dot(-ray.direction, lightNormal);
				colorAccumulator += material.CalculateDiffuseLighting(-ray.direction, incomingDirection,
																	  prim.GetNormal(ray.from),
																	  lightSource->material->GetEmissionColor() * intersectionRadianceFactor);
			}
		}
	}

	return (1.0f / (float)SCENE.emissiveRenderGroups.size()) * colorAccumulator;
}