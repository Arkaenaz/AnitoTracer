#include "Quad.h"

Quad::Quad()
	: Primitive("Quad")
{
}

Quad::Quad(std::string name)
	: Primitive(name)
{
}


DirectXUtil::Primitive::Shape Quad::createQuad(int size, bool uvHorizontalFlip, bool uvVerticalFlip, float uTileFactor,
	float vTileFactor)
{
	Shape returnQuadInfo;

	returnQuadInfo.vertexData =
	{
		// Indexed Quad
		DirectXUtil::Structs::VertexPositionNormalTangentTexture(glm::vec3(-size, 0, -size),
		                                                         glm::vec3(0, 1, 0),
		                                                         glm::vec3(0),
		                                                         glm::vec2(0)),
		DirectXUtil::Structs::VertexPositionNormalTangentTexture(glm::vec3(size, 0, -size),
		                                                         glm::vec3(0, 1, 0),
		                                                         glm::vec3(0),
		                                                         glm::vec2(0)),
		DirectXUtil::Structs::VertexPositionNormalTangentTexture(glm::vec3(size, 0, size),
		                                                         glm::vec3(0, 1, 0),
		                                                         glm::vec3(0),
		                                                         glm::vec2(0)),
		DirectXUtil::Structs::VertexPositionNormalTangentTexture(glm::vec3(-size, 0, size),
																 glm::vec3(0, 1, 0),
																 glm::vec3(0),
																 glm::vec2(0)),
	};

	returnQuadInfo.indexData = { 0, 1, 2, 0, 2, 3 };

	return returnQuadInfo;
}
