#include "SceneObjectFactory.h"

#include "../Rendering/Materials/LambertianMaterial.h"
#include "../Rendering/Materials/OrenNayarMaterial.h"
#include "../Geometry/Sphere.h"

void SceneObjectFactory::AddRoom(Scene & scene, bool addBackWalls, bool emissiveCeiling) {
	auto & materials = scene.materials;
	auto & renderGroups = scene.renderGroups;

	// Colors.
	const glm::vec3 whiteColor(1.0, 1.0, 1.0);
	const glm::vec3 redColor(1.0, 0.05f, 0.05f);
	const glm::vec3 cyanColor(0.05f, 1.0, 1.0);
	const glm::vec3 blueColor(0.05f, 0.05f, 1.0);
	const glm::vec3 greenColor(0.05f, 1.0, 0.05f);
	const glm::vec3 yellowColor(1.0, 1.0, 0.05f);
	const glm::vec3 orangeColor(1.0, 0.25, 0.05f);
	const glm::vec3 purpleColor(1.0, 0.05f, 1.0);

	// Materials.
	const auto whiteMaterial = new LambertianMaterial(whiteColor);
	materials.push_back(whiteMaterial);
	const auto redMaterial = new LambertianMaterial(redColor);
	materials.push_back(redMaterial);
	const auto greenMaterial = new LambertianMaterial(greenColor);
	materials.push_back(greenMaterial);
	const auto blueMaterial = new LambertianMaterial(blueColor);
	materials.push_back(blueMaterial);
	const auto yellowMaterial = new LambertianMaterial(yellowColor);
	materials.push_back(yellowMaterial);
	const auto orangeMaterial = new LambertianMaterial(orangeColor);
	materials.push_back(orangeMaterial);
	const auto purpleMaterial = new LambertianMaterial(purpleColor);
	materials.push_back(purpleMaterial);
	const auto emissiveMaterial = new LambertianMaterial(whiteColor, 1.0f);
	materials.push_back(emissiveMaterial);

	// -------------------
	// Ceiling.
	// -------------------
	RenderGroup ceiling(emissiveCeiling ? emissiveMaterial : whiteMaterial);

	glm::vec3 cv1(-3.0, 0.0, 5.0);
	glm::vec3 cv2(0.0, 6.0, 5.0);
	glm::vec3 cv3(10.0, 6.0, 5.0);
	glm::vec3 cv4(13.0, 0.0, 5.0);
	glm::vec3 cv5(10.0, -6.0, 5.0);
	glm::vec3 cv6(0.0, -6.0, 5.0);
	glm::vec3 ceilingNormal(0.0, 0.0, -1.0);

	if (addBackWalls) {
		ceiling.primitives.push_back(new Triangle(cv1, cv2, cv6, ceilingNormal));
	}
	ceiling.primitives.push_back(new Triangle(cv2, cv3, cv5, ceilingNormal));
	ceiling.primitives.push_back(new Triangle(cv2, cv5, cv6, ceilingNormal));
	ceiling.primitives.push_back(new Triangle(cv3, cv4, cv5, ceilingNormal));

	ceiling.RecalculateAABB();

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

	if (addBackWalls) {
		floor.primitives.push_back(new Triangle(fv1, fv2, fv6, floorNormal));
	}
	floor.primitives.push_back(new Triangle(fv2, fv3, fv5, floorNormal));
	floor.primitives.push_back(new Triangle(fv2, fv5, fv6, floorNormal));
	floor.primitives.push_back(new Triangle(fv3, fv4, fv5, floorNormal));

	floor.RecalculateAABB();

	renderGroups.push_back(floor);

	// -------------------
	// Wall 1.
	// -------------------
	if (addBackWalls) {

		RenderGroup wall1(yellowMaterial);

		glm::vec3 w1Normal = normalize(glm::vec3(2.0f, -1.0f, 0.0f));

		wall1.primitives.push_back(new Triangle(fv1, cv1, cv2, w1Normal));
		wall1.primitives.push_back(new Triangle(fv2, fv1, cv2, w1Normal));

		wall1.RecalculateAABB();

		renderGroups.push_back(wall1);
	}

	// -------------------
	// Wall 2.
	// -------------------
	RenderGroup wall2(greenMaterial);

	glm::vec3 w2Normal = normalize(glm::vec3(0.0, -1.0, 0.0));

	wall2.primitives.push_back(new Triangle(fv2, cv2, cv3, w2Normal));
	wall2.primitives.push_back(new Triangle(fv3, fv2, cv3, w2Normal));

	wall2.RecalculateAABB();

	renderGroups.push_back(wall2);

	// -------------------
	// Wall 3.
	// -------------------
	RenderGroup wall3(blueMaterial);

	glm::vec3 w3Normal = normalize(glm::vec3(-2.0, -1.0, 0.0));

	wall3.primitives.push_back(new Triangle(fv3, cv3, cv4, w3Normal));
	wall3.primitives.push_back(new Triangle(fv4, fv3, cv4, w3Normal));

	wall3.RecalculateAABB();

	renderGroups.push_back(wall3);

	// -------------------
	// Wall 4.
	// -------------------
	RenderGroup wall4(redMaterial);

	glm::vec3 w4Normal = normalize(glm::vec3(-2.0, 1.0, 0.0));

	wall4.primitives.push_back(new Triangle(fv4, cv4, cv5, w4Normal));
	wall4.primitives.push_back(new Triangle(fv5, fv4, cv5, w4Normal));

	wall4.RecalculateAABB();

	renderGroups.push_back(wall4);

	// -------------------
	// Wall 5.
	// -------------------
	RenderGroup wall5(orangeMaterial);

	glm::vec3 w5Normal = normalize(glm::vec3(0.0, 1.0, 0.0));

	wall5.primitives.push_back(new Triangle(fv5, cv5, cv6, w5Normal));
	wall5.primitives.push_back(new Triangle(fv6, fv5, cv6, w5Normal));

	wall5.RecalculateAABB();

	renderGroups.push_back(wall5);

	// -------------------
	// Wall 6.
	// -------------------
	if (addBackWalls) {

		RenderGroup wall6(purpleMaterial);

		glm::vec3 w6Normal = normalize(glm::vec3(2.0, 1.0, 0.0));

		wall6.primitives.push_back(new Triangle(fv6, cv6, cv1, w6Normal));
		wall6.primitives.push_back(new Triangle(fv1, fv6, cv1, w6Normal));

		wall6.RecalculateAABB();

		renderGroups.push_back(wall6);
	}
}

void SceneObjectFactory::AddOrenNayarSphere(Scene & scene, float x, float y, float z,
											float radius, glm::vec3 surfaceColor, float roughness) {
	auto & materials = scene.materials;
	auto & renderGroups = scene.renderGroups;

	// Material.
	const auto sphereMaterial = new OrenNayarMaterial(surfaceColor, roughness);
	materials.push_back(sphereMaterial);

	// Render group + primitive.
	RenderGroup sphereGroup(sphereMaterial);
	sphereGroup.primitives.push_back(new Sphere(glm::vec3(x, y, z), radius));
	sphereGroup.RecalculateAABB();
	renderGroups.push_back(sphereGroup);
}

void SceneObjectFactory::AddTriangle(Scene & scene, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
									 glm::vec3 normal, glm::vec3 surfaceColor, float emissivity) {
	auto & materials = scene.materials;
	auto & renderGroups = scene.renderGroups;

	// Material.
	const auto sphereMaterial = new LambertianMaterial(surfaceColor, emissivity);
	materials.push_back(sphereMaterial);

	// Render group + primitive.
	RenderGroup triangleGroup(sphereMaterial);
	triangleGroup.primitives.push_back(new Triangle(p1, p2, p3, normal));
	triangleGroup.RecalculateAABB();
	renderGroups.push_back(triangleGroup);
}

void SceneObjectFactory::Add2DQuad(Scene & scene, glm::vec2 corner1, glm::vec2 corner2, float height,
								   glm::vec3 normal, glm::vec3 surfaceColor, float emissivity) {
	auto & materials = scene.materials;
	auto & renderGroups = scene.renderGroups;

	glm::vec3 c1 = glm::vec3(corner1.x, corner1.y, height);
	glm::vec3 c2 = glm::vec3(corner1.x, corner2.y, height);
	glm::vec3 c3 = glm::vec3(corner2.x, corner2.y, height);
	glm::vec3 c4 = glm::vec3(corner2.x, corner1.y, height);

	// Material.
	const auto sphereMaterial = new LambertianMaterial(surfaceColor, emissivity);
	materials.push_back(sphereMaterial);

	// Render group + primitive.
	RenderGroup triangleGroup(sphereMaterial);

	triangleGroup.primitives.push_back(new Triangle(c1, c2, c3, normal));
	triangleGroup.primitives.push_back(new Triangle(c3, c4, c1, normal));
	triangleGroup.RecalculateAABB();
	renderGroups.push_back(triangleGroup);
}

void SceneObjectFactory::AddSphere(Scene & scene, float x, float y, float z,
								   float radius, glm::vec3 surfaceColor) {
	auto & materials = scene.materials;
	auto & renderGroups = scene.renderGroups;

	// Material.
	const auto sphereMaterial = new LambertianMaterial(surfaceColor);
	materials.push_back(sphereMaterial);

	// Render group + primitive.
	RenderGroup sphereGroup(sphereMaterial);
	sphereGroup.primitives.push_back(new Sphere(glm::vec3(x, y, z), radius));
	sphereGroup.RecalculateAABB();
	renderGroups.push_back(sphereGroup);
}

void SceneObjectFactory::AddTransparentSphere(Scene & scene, float x, float y, float z,
											  float radius, glm::vec3 surfaceColor,
											  float refractiveIndex, float transparency,
											  float reflectivity) {
	auto & materials = scene.materials;
	auto & renderGroups = scene.renderGroups;

	// Material.
	const auto transparentMaterial = new LambertianMaterial(surfaceColor, 0.0f, reflectivity, transparency, refractiveIndex);
	materials.push_back(transparentMaterial);

	// Render group + primitive.
	RenderGroup sphereGroup(transparentMaterial);
	sphereGroup.primitives.push_back(new Sphere(glm::vec3(x, y, z), radius));
	sphereGroup.RecalculateAABB();
	renderGroups.push_back(sphereGroup);
}

void SceneObjectFactory::AddTetrahedron(Scene & scene, float x, float y, float z, glm::vec3 surfaceColor,
										float emissivity, float refractiveIndex, float transparency,
										float reflectivity) {
	auto & materials = scene.materials;
	auto & renderGroups = scene.renderGroups;

	// Material.
	const auto tetraMaterial = new LambertianMaterial(surfaceColor, emissivity, reflectivity, transparency, refractiveIndex);
	materials.push_back(tetraMaterial);

	// Vertices.
	glm::vec3 v1(0.0 + x, 1.09 + y, 0.0 + z);
	glm::vec3 v2(0.0 + x, -0.54 + y, 1.15 + z);
	glm::vec3 v3(-1.0 + x, -0.54 + y, -0.57 + z);
	glm::vec3 v4(1.0 + x, -0.54 + y, -0.57 + z);

	// Normals.
	glm::vec3 n1 = normalize(glm::vec3(-2.8036, 1.15, 1.63));
	glm::vec3 n2 = normalize(glm::vec3(0.0, 1.14, -3.26));
	glm::vec3 n3 = normalize(glm::vec3(2.8036, 1.15, 1.63));
	glm::vec3 n4 = normalize(glm::vec3(0.0, -3.44, 0.0));

	// Render group + primitives.
	RenderGroup tetrahedronGroup(tetraMaterial);

	// Add triangles.
	tetrahedronGroup.primitives.push_back(new Triangle(v1, v3, v2, n1));
	tetrahedronGroup.primitives.push_back(new Triangle(v1, v4, v3, n2));
	tetrahedronGroup.primitives.push_back(new Triangle(v2, v4, v1, n3));
	tetrahedronGroup.primitives.push_back(new Triangle(v3, v4, v2, n4));

	tetrahedronGroup.RecalculateAABB();

	renderGroups.push_back(tetrahedronGroup);
}

void SceneObjectFactory::AddEmissiveSphere(Scene & scene, float x, float y, float z, float radius,
										   glm::vec3 surfaceColor, float emissivity) {
	auto & materials = scene.materials;
	auto & renderGroups = scene.renderGroups;

	// Material.
	const auto mat = new LambertianMaterial(surfaceColor, emissivity);
	materials.push_back(mat);

	// Render group + primitive.
	RenderGroup sphereGroup(mat);
	sphereGroup.primitives.push_back(new Sphere(glm::vec3(x, y, z), radius));
	sphereGroup.RecalculateAABB();
	renderGroups.push_back(sphereGroup);
}
