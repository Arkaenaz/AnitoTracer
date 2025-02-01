#include "Cube.h"

#include "Source/Window.h"

Cube::Cube()
	: Primitive("Cube")
{
}

Cube::Cube(std::string name)
	: Primitive(name)
{
}

DirectXUtil::Primitive::Shape Cube::createCube(float size, bool uvHorizontalFlip, bool uvVerticalFlip,
	float uTileFactor, float vTileFactor)
{
	Shape returnCubeInfo;

	const float uCoordMin = uvHorizontalFlip ? uTileFactor : 0;
	const float uCoordMax = uvHorizontalFlip ? 0 : uTileFactor;
	const float vCoordMin = uvVerticalFlip ? vTileFactor : 0;
	const float vCoordMax = uvVerticalFlip ? 0 : vTileFactor;

	const glm::vec3 normals[] = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),
		glm::vec3(1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0),
	};

	const glm::vec2 texCoord[] = {
		glm::vec2(uCoordMax, vCoordMax), glm::vec2(uCoordMin, vCoordMax), glm::vec2(uCoordMin, vCoordMin),
		glm::vec2(uCoordMax, vCoordMin),
		glm::vec2(uCoordMin, vCoordMin), glm::vec2(uCoordMax, vCoordMin), glm::vec2(uCoordMax, vCoordMax),
		glm::vec2(uCoordMin, vCoordMax),
		glm::vec2(uCoordMax, vCoordMin), glm::vec2(uCoordMax, vCoordMax), glm::vec2(uCoordMin, vCoordMax),
		glm::vec2(uCoordMin, vCoordMin),
		glm::vec2(uCoordMax, vCoordMin), glm::vec2(uCoordMax, vCoordMax), glm::vec2(uCoordMin, vCoordMax),
		glm::vec2(uCoordMin, vCoordMin),
		glm::vec2(uCoordMin, vCoordMax), glm::vec2(uCoordMin, vCoordMin), glm::vec2(uCoordMax, vCoordMin),
		glm::vec2(uCoordMax, vCoordMax),
		glm::vec2(uCoordMax, vCoordMin), glm::vec2(uCoordMax, vCoordMax), glm::vec2(uCoordMin, vCoordMax),
		glm::vec2(uCoordMin, vCoordMin),
	};

	const glm::vec3 tangents[] = {
		glm::vec3(1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
	};

	// Create each face in turn.
	for (int i = 0, j = 0; i < NV_ARRAYSIZE(normals); i++, j += 4)
	{
		glm::vec3 normal = normals[i];
		glm::vec3 tangent = tangents[i];

		// Get two vectors perpendicular to the face normal and to each other.
		glm::vec3 side1 = glm::vec3(normal.y, normal.z, normal.x);
		glm::vec3 side2 = glm::cross(normal, side1);

		const int vertexCount = returnCubeInfo.vertexData.size();

		// Six indices (two triangles) per face.
		returnCubeInfo.indexData.push_back(static_cast<unsigned short>(vertexCount + 0));

		returnCubeInfo.indexData.push_back(static_cast<unsigned short>(vertexCount + 1));

		returnCubeInfo.indexData.push_back(static_cast<unsigned short>(vertexCount + 3));

		returnCubeInfo.indexData.push_back(static_cast<unsigned short>(vertexCount + 1));

		returnCubeInfo.indexData.push_back(static_cast<unsigned short>(vertexCount + 2));

		returnCubeInfo.indexData.push_back(static_cast<unsigned short>(vertexCount + 3));

		// 0   3
		// 1   2
		const float sideOverTwo = size * 0.5f;

		// Four vertices per face.
		returnCubeInfo.vertexData.emplace_back(
			(normal - side1 - side2) * sideOverTwo,
			normal,
			tangent,
			texCoord[j]
		);

		returnCubeInfo.vertexData.emplace_back(
			(normal - side1 + side2) * sideOverTwo,
			normal,
			tangent,
			texCoord[j + 1]
		);

		returnCubeInfo.vertexData.emplace_back(
			(normal + side1 + side2) * sideOverTwo,
			normal,
			tangent,
			texCoord[j + 2]
		);

		returnCubeInfo.vertexData.emplace_back(
			(normal + side1 - side2) * sideOverTwo,
			normal,
			tangent,
			texCoord[j + 3]
		);

		// this should be temporary
		returnCubeInfo.otherVertexData.emplace_back(
			(normal - side1 - side2) * sideOverTwo,
			glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
		);

		returnCubeInfo.otherVertexData.emplace_back(
			(normal - side1 + side2) * sideOverTwo,
			glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
		);

		returnCubeInfo.otherVertexData.emplace_back(
			(normal + side1 + side2) * sideOverTwo,
			glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
		);

		returnCubeInfo.otherVertexData.emplace_back(
			(normal + side1 - side2) * sideOverTwo,
			glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
		);
	}

	calculateTangentSpace(returnCubeInfo);

	return returnCubeInfo;
}
