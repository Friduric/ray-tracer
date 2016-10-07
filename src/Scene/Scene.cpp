#include "Scene.h"

#include <memory>
#include <algorithm>
#include <cassert>

#include "../../includes/glm/gtx/norm.hpp"
#include "../Rendering/Materials/LambertianMaterial.h"
#include "../Geometry/Sphere.h"

Scene::Scene() {}

Scene::~Scene()
{
	for (auto& rg : renderGroups) {
		for (auto& prim : rg.primitives) {
			delete prim;
		}
	}
	for (auto m : materials) {
		delete m;
	}
}

glm::vec3 Scene::TraceRay(const Ray & ray, const unsigned int BOUNCES_PER_HIT,
						  const unsigned int MAX_DEPTH, const float DEPRECATION_FACTOR) const {
	if (MAX_DEPTH == 0) { return glm::vec3(0, 0, 0); }

	assert(BOUNCES_PER_HIT > 0);

	unsigned int intersectionPrimitiveIndex;
	unsigned int intersectionRenderGroupIndex;
	float intersectionDistance;
	bool intersectionFound = RayCast(ray, intersectionRenderGroupIndex,
									 intersectionPrimitiveIndex, intersectionDistance);

	/* If the ray doesn't intersect, simply return (0, 0, 0). */
	if (!intersectionFound) { return glm::vec3(0, 0, 0); }

	const auto& intersectionRenderGroup = renderGroups[intersectionRenderGroupIndex];

	/* Simplification: emissive materials only return their emission. I.e. no indirect lighting
	 * from other lights. */
	if (glm::length(intersectionRenderGroup.material->GetEmissionColor()) > FLT_EPSILON) {
		return intersectionRenderGroup.material->GetEmissionColor();
	}

	const auto& intersectionPrimitive = intersectionRenderGroup.primitives[intersectionPrimitiveIndex];
	glm::vec3 intersectionPoint = ray.from + ray.dir * intersectionDistance;
	glm::vec3 hitNormal = intersectionPrimitive->GetNormal(intersectionPoint);
	Material* hitMaterial = intersectionRenderGroup.material;

	/* We intersected with something non-emissive. Now shoot rays all over the place. */
	glm::vec3 colorAccumulator = { 0,0,0 };
	std::vector<Ray> bouncingRays = GenerateBouncingRays(hitNormal,
														 intersectionPoint,
														 hitMaterial,
														 BOUNCES_PER_HIT);
	int nextBouncesPerHit = BOUNCES_PER_HIT;
	nextBouncesPerHit -= (float)(BOUNCES_PER_HIT / MAX_DEPTH) * DEPRECATION_FACTOR;
	for (const auto & bouncedRay : bouncingRays) {
		colorAccumulator += TraceRay(ray, std::min(nextBouncesPerHit, 0), MAX_DEPTH - 1, DEPRECATION_FACTOR);
	}
	return (1.0f / (float)BOUNCES_PER_HIT) * colorAccumulator;
}


bool Scene::RayCast(const Ray & ray, unsigned int & intersectionRenderGroupIndex,
					unsigned int & intersectionPrimitiveIndex, float & intersectionDistance) const {
	float closestInterectionDistance = FLT_MAX;
	for (unsigned int i = 0; i < renderGroups.size(); ++i) {
		const auto& rg = renderGroups[i];
		for (unsigned int j = 0; j < rg.primitives.size(); ++j) {
			const auto& pr = rg.primitives[j];
			/* Check if the ray intersects with anything. */
			bool intersects = pr->RayIntersection(ray, intersectionDistance);
			if (intersects) {
				if (intersectionDistance < closestInterectionDistance) {
					intersectionRenderGroupIndex = i;
					intersectionPrimitiveIndex = j;
					closestInterectionDistance = intersectionDistance;
				}
			}
		}
	}
	return closestInterectionDistance < FLT_MAX - FLT_EPSILON;
}

void Scene::CreateRoom() {

	// Colors.
	const glm::vec3 whiteColor(1.0, 1.0, 1.0);
	const glm::vec3 redColor(1.0, 0.0, 0.0);
	const glm::vec3 cyanColor(0.0, 1.0, 1.0);
	const glm::vec3 blueColor(0.0, 0.0, 1.0);
	const glm::vec3 greenColor(0.0, 1.0, 0.0);
	const glm::vec3 yellowColor(1.0, 1.0, 0.0);
	const glm::vec3 purpleColor(1.0, 0.0, 1.0);

	// Materials.
	const auto whiteMaterial = new LambertianMaterial(whiteColor);
	materials.push_back(whiteMaterial);
	const auto redMaterial = new LambertianMaterial(redColor);
	materials.push_back(redMaterial);
	const auto greenMaterial = new LambertianMaterial(cyanColor);
	materials.push_back(greenMaterial);
	const auto blueMaterial = new LambertianMaterial(blueColor);
	materials.push_back(blueMaterial);
	const auto yellowMaterial = new LambertianMaterial(greenColor);
	materials.push_back(yellowMaterial);
	const auto orangeMaterial = new LambertianMaterial(yellowColor);
	materials.push_back(orangeMaterial);
	const auto purpleMaterial = new LambertianMaterial(purpleColor);
	materials.push_back(purpleMaterial);

	// -------------------
	// Ceiling.
	// -------------------
	RenderGroup ceiling(whiteMaterial);

	glm::vec3 cv1(-3.0, 0.0, 5.0);
	glm::vec3 cv2(0.0, 6.0, 5.0);
	glm::vec3 cv3(10.0, 6.0, 5.0);
	glm::vec3 cv4(13.0, 0.0, 5.0);
	glm::vec3 cv5(10.0, -6.0, 5.0);
	glm::vec3 cv6(0.0, -6.0, 5.0);
	glm::vec3 ceilingNormal(0.0, 0.0, -1.0);

	ceiling.primitives.push_back(new Triangle(cv1, cv2, cv6, ceilingNormal));
	ceiling.primitives.push_back(new Triangle(cv2, cv3, cv5, ceilingNormal));
	ceiling.primitives.push_back(new Triangle(cv2, cv5, cv6, ceilingNormal));
	ceiling.primitives.push_back(new Triangle(cv3, cv4, cv5, ceilingNormal));

	renderGroups.push_back(ceiling);

	// -------------------
	// Floor.
	// -------------------
	RenderGroup floor(whiteMaterial);

	glm::vec3 fv1(-3.0, 0.0, -5.0);
	glm::vec3 fv2(0.0, 6.0, -5.0);
	glm::vec3 fv3(10.0, 6.0, -5.0);
	glm::vec3 fv4(13.0, 0.0, -5.0);
	glm::vec3 fv5(10.0, -6.0, -5.0);
	glm::vec3 fv6(0.0, -6.0, -5.0);
	glm::vec3 floorNormal(0.0, 0.0, 1.0);

	floor.primitives.push_back(new Triangle(fv1, fv2, fv6, floorNormal));
	floor.primitives.push_back(new Triangle(fv2, fv3, fv5, floorNormal));
	floor.primitives.push_back(new Triangle(fv2, fv5, fv6, floorNormal));
	floor.primitives.push_back(new Triangle(fv3, fv4, fv5, floorNormal));

	renderGroups.push_back(floor);

	// -------------------
	// Wall 1.
	// -------------------
	RenderGroup wall1(redMaterial);

	glm::vec3 w1Normal(2.0, -1.0, 0.0);

	wall1.primitives.push_back(new Triangle(fv1, cv1, cv2, w1Normal));
	wall1.primitives.push_back(new Triangle(fv2, fv1, cv2, w1Normal));

	renderGroups.push_back(wall1);

	// -------------------
	// Wall 2.
	// -------------------
	RenderGroup wall2(greenMaterial);

	glm::vec3 w2Normal(0.0, -1.0, 0.0);

	wall2.primitives.push_back(new Triangle(fv2, cv2, cv3, w2Normal));
	wall2.primitives.push_back(new Triangle(fv3, fv2, cv3, w2Normal));

	renderGroups.push_back(wall2);

	// -------------------
	// Wall 3.
	// -------------------
	RenderGroup wall3(blueMaterial);

	glm::vec3 w3Normal(-2.0, -1.0, 0.0);

	wall3.primitives.push_back(new Triangle(fv3, cv3, cv4, w3Normal));
	wall3.primitives.push_back(new Triangle(fv4, fv3, cv4, w3Normal));

	renderGroups.push_back(wall3);

	// -------------------
	// Wall 4.
	// -------------------
	RenderGroup wall4(yellowMaterial);

	glm::vec3 w4Normal(-2.0, 1.0, 0.0);

	wall4.primitives.push_back(new Triangle(fv4, cv4, cv5, w4Normal));
	wall4.primitives.push_back(new Triangle(fv5, fv4, cv5, w4Normal));

	renderGroups.push_back(wall4);

	// -------------------
	// Wall 5.
	// -------------------
	RenderGroup wall5(orangeMaterial);

	glm::vec3 w5Normal(0.0, 1.0, 0.0);

	wall5.primitives.push_back(new Triangle(fv5, cv5, cv6, w5Normal));
	wall5.primitives.push_back(new Triangle(fv6, fv5, cv6, w5Normal));

	renderGroups.push_back(wall5);

	// -------------------
	// Wall 6.
	// -------------------
	RenderGroup wall6(purpleMaterial);

	glm::vec3 w6Normal(2.0, 1.0, 0.0);

	wall6.primitives.push_back(new Triangle(fv6, cv6, cv1, w6Normal));
	wall6.primitives.push_back(new Triangle(fv1, fv6, cv1, w6Normal));

	renderGroups.push_back(wall6);
}

void Scene::CreateTetrahedron(float x, float y, float z) {

	// Material.
	glm::vec3 c(0.2, 0.5, 0.5);
	const auto tetraMaterial = new LambertianMaterial(c);
	materials.push_back(tetraMaterial);

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

	// Render group + primitives.
	RenderGroup tetrahedronGroup(tetraMaterial);

	// Add triangles.
	tetrahedronGroup.primitives.push_back(new Triangle(v1, v3, v2, n1));
	tetrahedronGroup.primitives.push_back(new Triangle(v1, v4, v3, n2));
	tetrahedronGroup.primitives.push_back(new Triangle(v2, v4, v1, n3));
	tetrahedronGroup.primitives.push_back(new Triangle(v3, v4, v2, n4));

	renderGroups.push_back(tetrahedronGroup);
}

void Scene::CreateSphere(float x, float y, float z, float radius) {

	// Material.
	glm::vec3 c(0.5, 0.5, 0.0);
	const auto sphereMaterial = new LambertianMaterial(c);
	materials.push_back(sphereMaterial);

	// Render group + primitive.
	RenderGroup sphereGroup(sphereMaterial);
	sphereGroup.primitives.push_back(new Sphere(glm::vec3(x, y, z), radius));
	renderGroups.push_back(sphereGroup);
}

void Scene::CreateEmissiveSphere(float x, float y, float z, float radius, glm::vec3 emissionColor) {

	// Material.
	glm::vec3 c(1.0, 1.0, 1.0);
	const auto sphereMaterial = new LambertianMaterial(c, emissionColor);
	materials.push_back(sphereMaterial);

	// Render group + primitive.
	RenderGroup sphereGroup(sphereMaterial);
	sphereGroup.primitives.push_back(new Sphere(glm::vec3(x, y, z), radius));
	renderGroups.push_back(sphereGroup);
}

std::vector<Ray> Scene::GenerateBouncingRays(const glm::vec3 & hitNormal,
											 const glm::vec3 & intersectionPoint,
											 const Material * material,
											 const unsigned int numberOfRays) const
{
	return std::vector<Ray>();
}
