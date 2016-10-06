#include "Scene.h"

#include "../../includes/glm/gtx/norm.hpp"

Scene::Scene() {}

glm::vec3 Scene::TraceRay(const Ray & ray, const unsigned int BOUNCES_PER_HIT,
						  const unsigned int MAX_DEPTH) const {
	if (MAX_DEPTH == 0) { return glm::vec3(0, 0, 0); }

	unsigned int intersectionPrimitiveIndex;
	unsigned int intersectionRenderGroupIndex;
	glm::vec3 intersectionPoint;
	bool intersectionFound = RayCast(ray, intersectionRenderGroupIndex,
									 intersectionPrimitiveIndex, intersectionPoint);

	/* If the ray doesn't intersect, simply return (0, 0, 0). */
	if (!intersectionFound) { return glm::vec3(0, 0, 0); }

	const auto& intersectionRenderGroup = renderGroups[intersectionRenderGroupIndex];

	/* Simplification: emissive materials only return their emission. I.e. no indirect lighting
	 * from other lights. */
	if (glm::length(intersectionRenderGroup.material->emission) > FLT_EPSILON) {
		return intersectionRenderGroup.material->emission;
	}

	const auto& intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
	/* We intersected with something non-emissive. Now shoot rays all over the place. */
	glm::vec3 colorAccumulator = { 0,0,0 };
	glm::vec3 hitNormal = intersectionPrimitive.GetNormal(intersectionPoint);

	// Testing.
	return intersectionPrimitive.color;

	/*
	for (unsigned int b = 0; b < BOUNCES_PER_HIT; ++b) {

	}

	return colorAccumulator;
	*/
}

bool Scene::RayCast(const Ray & ray, unsigned int & intersectionRenderGroupIndex,
					unsigned int & intersectionPrimitiveIndex, glm::vec3 & intersectionPoint) const {
	float closestInterectionDistance = FLT_MAX;
	for (unsigned int i = 0; i < renderGroups.size(); ++i) {
		const auto& rg = renderGroups[i];
		for (unsigned int j = 0; j < rg.primitives.size(); ++j) {
			const auto& pr = rg.primitives[j];
			/* Check if the ray intersects with anything. */
			bool intersects = pr.RayIntersection(ray, intersectionPoint);
			if (intersects) {
				float distance = glm::distance2(ray.from, intersectionPoint);
				if (distance < closestInterectionDistance) {
					intersectionRenderGroupIndex = i;
					intersectionPrimitiveIndex = j;
					closestInterectionDistance = distance;
				}
			}
		}
	}
	return closestInterectionDistance < FLT_MAX - FLT_EPSILON;
}

void Scene::CreateRoom() {
	glm::vec3 whiteColor(1.0, 1.0, 1.0);

	// Ceiling.
	glm::vec3 cv1(-3.0, 0.0, 5.0);
	glm::vec3 cv2(0.0, 6.0, 5.0);
	glm::vec3 cv3(10.0, 6.0, 5.0);
	glm::vec3 cv4(13.0, 0.0, 5.0);
	glm::vec3 cv5(10.0, -6.0, 5.0);
	glm::vec3 cv6(0.0, -6.0, 5.0);
	glm::vec3 ceilingNormal(0.0, 0.0, -1.0);
	Triangle ct1(cv1, cv2, cv6, whiteColor, ceilingNormal);
	Triangle ct2(cv2, cv3, cv5, whiteColor, ceilingNormal);
	Triangle ct3(cv2, cv5, cv6, whiteColor, ceilingNormal);
	Triangle ct4(cv3, cv4, cv5, whiteColor, ceilingNormal);
	triangles.push_back(ct1);
	triangles.push_back(ct2);
	triangles.push_back(ct3);
	triangles.push_back(ct4);

	// Floor.
	glm::vec3 fv1(-3.0, 0.0, -5.0);
	glm::vec3 fv2(0.0, 6.0, -5.0);
	glm::vec3 fv3(10.0, 6.0, -5.0);
	glm::vec3 fv4(13.0, 0.0, -5.0);
	glm::vec3 fv5(10.0, -6.0, -5.0);
	glm::vec3 fv6(0.0, -6.0, -5.0);
	glm::vec3 floorNormal(0.0, 0.0, 1.0);
	Triangle ft1(fv1, fv2, fv6, whiteColor, floorNormal);
	Triangle ft2(fv2, fv3, fv5, whiteColor, floorNormal);
	Triangle ft3(fv2, fv5, fv6, whiteColor, floorNormal);
	Triangle ft4(fv3, fv4, fv5, whiteColor, floorNormal);
	triangles.push_back(ft1);
	triangles.push_back(ft2);
	triangles.push_back(ft3);
	triangles.push_back(ft4);

	// Wall 1.
	glm::vec3 redColor(1.0, 0.0, 0.0);
	glm::vec3 w1Normal(2.0, -1.0, 0.0);
	Triangle w1t1(fv1, cv1, cv2, redColor, w1Normal);
	Triangle w1t2(fv2, fv1, cv2, redColor, w1Normal);
	triangles.push_back(w1t1);
	triangles.push_back(w1t2);

	// Wall 2.
	glm::vec3 greenColor(0.0, 1.0, 1.0);
	glm::vec3 w2Normal(0.0, -1.0, 0.0);
	Triangle w2t1(fv2, cv2, cv3, greenColor, w2Normal);
	Triangle w2t2(fv3, fv2, cv3, greenColor, w2Normal);
	triangles.push_back(w2t1);
	triangles.push_back(w2t2);

	// Wall 3.
	glm::vec3 blueColor(0.0, 0.0, 1.0);
	glm::vec3 w3Normal(-2.0, -1.0, 0.0);
	Triangle w3t1(fv3, cv3, cv4, blueColor, w3Normal);
	Triangle w3t2(fv4, fv3, cv4, blueColor, w3Normal);
	triangles.push_back(w3t1);
	triangles.push_back(w3t2);

	// Wall 4.
	glm::vec3 yellowColor(0.0, 1.0, 0.0);
	glm::vec3 w4Normal(-2.0, 1.0, 0.0);
	Triangle w4t1(fv4, cv4, cv5, yellowColor, w4Normal);
	Triangle w4t2(fv5, fv4, cv5, yellowColor, w4Normal);
	triangles.push_back(w4t1);
	triangles.push_back(w4t2);

	// Wall 5.
	glm::vec3 orangeColor(1.0, 1.0, 0.0);
	glm::vec3 w5Normal(0.0, 1.0, 0.0);
	Triangle w5t1(fv5, cv5, cv6, orangeColor, w5Normal);
	Triangle w5t2(fv6, fv5, cv6, orangeColor, w5Normal);
	triangles.push_back(w5t1);
	triangles.push_back(w5t2);

	// Wall 6.
	glm::vec3 purpleColor(1.0, 0.0, 1.0);
	glm::vec3 w6Normal(2.0, 1.0, 0.0);
	Triangle w6t1(fv6, cv6, cv1, purpleColor, w6Normal);
	Triangle w6t2(fv1, fv6, cv1, purpleColor, w6Normal);
	triangles.push_back(w6t1);
	triangles.push_back(w6t2);
}

void Scene::CreateTetrahedron(float x, float y, float z) {
	glm::vec3 c(1.0, 0.0, 0.0);

	// Vertices.
	glm::vec3 v1(0.0 + x, 1.09 + y, 0.0 + z);
	glm::vec3 v2(0.0 + x, -0.54 + y, 1.15 + z);
	glm::vec3 v3(-1.0 + x, -0.54 + y, -0.57 + z);
	glm::vec3 v4(1.0 + x, -0.54 + y, -0.57 + z);

	// Normals.
	glm::vec3 n1(-2.8036, 1.15, 1.63);
	glm::vec3 n2(0.0, 1.14, -3.26);
	glm::vec3 n3(2.8036, 1.15, 1.63);
	glm::vec3 n4(0.0, -3.44, 0.0);

	// Triangles.
	Triangle t1(v1, v3, v2, c, n1);
	Triangle t2(v1, v4, v3, c, n2);
	Triangle t3(v2, v4, v1, c, n3);
	Triangle t4(v3, v4, v2, c, n4);

	// Add triangles.
	triangles.push_back(t1);
	triangles.push_back(t2);
	triangles.push_back(t3);
	triangles.push_back(t4);
}
