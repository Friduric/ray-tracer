#include "Scene.h"
#include <memory>
#include "../../includes/glm/gtx/norm.hpp"
#include "../Rendering/Material/LambertianMaterial.h";
#include "../Geometry/Sphere.h"

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
	if (glm::length(intersectionRenderGroup.material->GetEmissionColor()) > FLT_EPSILON) {
		return intersectionRenderGroup.material->GetEmissionColor();
	}
	else {
		return intersectionRenderGroup.material->GetSurfaceColor(); // Only for testing. Remove this.
	}

	/*

	const auto& intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
	// We intersected with something non-emissive. Now shoot rays all over the place.
	glm::vec3 colorAccumulator = { 0,0,0 };
	glm::vec3 hitNormal = intersectionPrimitive.GetNormal(intersectionPoint);

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

/*void Scene::CreatePhotonMap() {
	// Get all light sources
	std::vector<RenderGroup> lightSources;

}*/

void Scene::CreateRoom() {
	glm::vec3 whiteColor(1.0, 1.0, 1.0);
	auto whiteMaterial = std::make_shared<Material>(LambertianMaterial(whiteColor));

	// Ceiling.
	glm::vec3 cv1(-3.0, 0.0, 5.0);
	glm::vec3 cv2(0.0, 6.0, 5.0);
	glm::vec3 cv3(10.0, 6.0, 5.0);
	glm::vec3 cv4(13.0, 0.0, 5.0);
	glm::vec3 cv5(10.0, -6.0, 5.0);
	glm::vec3 cv6(0.0, -6.0, 5.0);
	glm::vec3 ceilingNormal(0.0, 0.0, -1.0);
	RenderGroup ceiling(whiteMaterial);
	Triangle ct1(cv1, cv2, cv6, ceilingNormal);
	Triangle ct2(cv2, cv3, cv5, ceilingNormal);
	Triangle ct3(cv2, cv5, cv6, ceilingNormal);
	Triangle ct4(cv3, cv4, cv5, ceilingNormal);
	ceiling.primitives.push_back(ct1);
	ceiling.primitives.push_back(ct2);
	ceiling.primitives.push_back(ct3);
	ceiling.primitives.push_back(ct4);
	renderGroups.push_back(ceiling);

	// Floor.
	glm::vec3 fv1(-3.0, 0.0, -5.0);
	glm::vec3 fv2(0.0, 6.0, -5.0);
	glm::vec3 fv3(10.0, 6.0, -5.0);
	glm::vec3 fv4(13.0, 0.0, -5.0);
	glm::vec3 fv5(10.0, -6.0, -5.0);
	glm::vec3 fv6(0.0, -6.0, -5.0);
	glm::vec3 floorNormal(0.0, 0.0, 1.0);
	RenderGroup floor(whiteMaterial);
	Triangle ft1(fv1, fv2, fv6, floorNormal);
	Triangle ft2(fv2, fv3, fv5, floorNormal);
	Triangle ft3(fv2, fv5, fv6, floorNormal);
	Triangle ft4(fv3, fv4, fv5, floorNormal);
	floor.primitives.push_back(ft1);
	floor.primitives.push_back(ft2);
	floor.primitives.push_back(ft3);
	floor.primitives.push_back(ft4);
	renderGroups.push_back(floor);

	// Wall 1.
	glm::vec3 redColor(1.0, 0.0, 0.0);
	glm::vec3 w1Normal(2.0, -1.0, 0.0);
	auto redMaterial = std::make_shared<Material>(LambertianMaterial(redColor));
	RenderGroup wall1(redMaterial);
	Triangle w1t1(fv1, cv1, cv2, w1Normal);
	Triangle w1t2(fv2, fv1, cv2, w1Normal);
	wall1.primitives.push_back(w1t1);
	wall1.primitives.push_back(w1t2);
	renderGroups.push_back(wall1);

	// Wall 2.
	glm::vec3 greenColor(0.0, 1.0, 1.0);
	glm::vec3 w2Normal(0.0, -1.0, 0.0);
	auto greenMaterial = std::make_shared<Material>(LambertianMaterial(greenColor));
	RenderGroup wall2(greenMaterial);
	Triangle w2t1(fv2, cv2, cv3, w2Normal);
	Triangle w2t2(fv3, fv2, cv3, w2Normal);
	wall2.primitives.push_back(w2t1);
	wall2.primitives.push_back(w2t2);
	renderGroups.push_back(wall2);

	// Wall 3.
	glm::vec3 blueColor(0.0, 0.0, 1.0);
	glm::vec3 w3Normal(-2.0, -1.0, 0.0);
	auto greenMaterial = std::make_shared<Material>(LambertianMaterial(greenColor));
	RenderGroup wall3(greenMaterial);
	Triangle w3t1(fv3, cv3, cv4, w3Normal);
	Triangle w3t2(fv4, fv3, cv4, w3Normal);
	wall3.primitives.push_back(w3t1);
	wall3.primitives.push_back(w3t2);
	renderGroups.push_back(wall3);

	// Wall 4.
	glm::vec3 yellowColor(0.0, 1.0, 0.0);
	glm::vec3 w4Normal(-2.0, 1.0, 0.0);
	auto yellowMaterial = std::make_shared<Material>(LambertianMaterial(yellowColor));
	RenderGroup wall4(yellowMaterial);
	Triangle w4t1(fv4, cv4, cv5, w4Normal);
	Triangle w4t2(fv5, fv4, cv5, w4Normal);
	wall4.primitives.push_back(w4t1);
	wall4.primitives.push_back(w4t2);
	renderGroups.push_back(wall4);

	// Wall 5.
	glm::vec3 orangeColor(1.0, 1.0, 0.0);
	glm::vec3 w5Normal(0.0, 1.0, 0.0);
	auto orangeMaterial = std::make_shared<Material>(LambertianMaterial(orangeColor));
	RenderGroup wall5(orangeMaterial);
	Triangle w5t1(fv5, cv5, cv6, w5Normal);
	Triangle w5t2(fv6, fv5, cv6, w5Normal);
	wall5.primitives.push_back(w5t1);
	wall5.primitives.push_back(w5t2);
	renderGroups.push_back(wall5);

	// Wall 6.
	glm::vec3 purpleColor(1.0, 0.0, 1.0);
	glm::vec3 w6Normal(2.0, 1.0, 0.0);
	auto purpleMaterial = std::make_shared<Material>(LambertianMaterial(purpleColor));
	RenderGroup wall6(purpleMaterial);
	Triangle w6t1(fv6, cv6, cv1, w6Normal);
	Triangle w6t2(fv1, fv6, cv1, w6Normal);
	wall6.primitives.push_back(w6t1);
	wall6.primitives.push_back(w6t2);
	renderGroups.push_back(wall6);
}

void Scene::CreateTetrahedron(float x, float y, float z) {
	glm::vec3 c(0.0, 0.5, 0.5);
	auto tetraMat = std::make_shared<Material>(LambertianMaterial(c));
	RenderGroup tetrahedron(tetraMat);

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
	Triangle t1(v1, v3, v2, n1);
	Triangle t2(v1, v4, v3, n2);
	Triangle t3(v2, v4, v1, n3);
	Triangle t4(v3, v4, v2, n4);

	// Add triangles.
	tetrahedron.primitives.push_back(t1);
	tetrahedron.primitives.push_back(t2);
	tetrahedron.primitives.push_back(t3);
	tetrahedron.primitives.push_back(t4);

	renderGroups.push_back(tetrahedron);
}

void Scene::CreateSphere(float x, float y, float z, float radius) {
	glm::vec3 c(0.5, 0.5, 0.0);
	auto sphereMat = std::make_shared<Material>(LambertianMaterial(c));
	RenderGroup sphereGroup(sphereMat);
	Sphere sphere(glm::vec3(x,y,z), radius);

	renderGroups.push_back(sphereGroup);
}
