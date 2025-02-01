#include "Primitive.h"

//#define _USE_MATH_DEFINES
//Defined in the project
#include <cmath>

#include "Source/Object.h"
#include "Source/Object.h"
#include "_externals/glm/glm/fwd.hpp"
#include "_externals/glm/glm/geometric.hpp"
#include "Source/Window.h"

DirectXUtil::Primitive::Primitive()
	: Object("Primitive", ObjectType::primitive)
{
}

DirectXUtil::Primitive::Primitive(std::string name)
	: Object("name", ObjectType::primitive)
{
}

void DirectXUtil::Primitive::calculateTangentSpace(Shape& shape)
{
	const int vertexCount = shape.vertexData.size();
	const int triangleCount = shape.indexData.size() / 3;

	glm::vec3* tan1 = new glm::vec3[vertexCount * 2];
	glm::vec3* tan2 = tan1 + vertexCount;

	Structs::VertexPositionNormalTangentTexture a1, a2, a3;
	glm::vec3 v1, v2, v3;
	glm::vec2 w1, w2, w3;

	for (int a = 0; a < triangleCount; a++)
	{
		const unsigned short i1 = shape.indexData[(a * 3) + 0];
		const unsigned short i2 = shape.indexData[(a * 3) + 1];
		const unsigned short i3 = shape.indexData[(a * 3) + 2];

		a1 = shape.vertexData[i1];
		a2 = shape.vertexData[i2];
		a3 = shape.vertexData[i3];

		v1 = a1.position;
		v2 = a2.position;
		v3 = a3.position;

		w1 = a1.texCoord;
		w2 = a2.texCoord;
		w3 = a3.texCoord;

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		const float r = 1.0F / ((s1 * t2) - (s2 * t1));
		glm::vec3 sdir(((t2 * x1) - (t1 * x2)) * r, ((t2 * y1) - (t1 * y2)) * r, ((t2 * z1) - (t1 * z2)) * r);
		glm::vec3 tdir(((s1 * x2) - (s2 * x1)) * r, ((s1 * y2) - (s2 * y1)) * r, ((s1 * z2) - (s2 * z1)) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (int a = 0; a < vertexCount; a++)
	{
		Structs::VertexPositionNormalTangentTexture vertex = shape.vertexData[a];

		const glm::vec3 n = vertex.normal;
		const glm::vec3 t = tan1[a];

		// Gram-Schmidt orthogonalize
		vertex.tangent = t - (n * glm::dot(n, t));
		vertex.tangent = glm::normalize(vertex.tangent);

		shape.vertexData[a] = vertex;
	}
}
