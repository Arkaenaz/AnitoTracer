#pragma once

#include <vector>

#include "Source/Object.h"
#include "Source/Utils/Structs/VertexPositionNormalTangentTexture.h"

namespace DirectXUtil
{
	class Primitive : public Object
	{
	public:
		Primitive();
		Primitive(std::string name);

		/**
		 * General Shape information
		 */
		struct Shape
		{
			std::vector<Structs::VertexPositionNormalTangentTexture> vertexData;
			std::vector<Structs::VertexPositionColor> otherVertexData;
			std::vector<unsigned short> indexData;
		};

	protected:
		static void calculateTangentSpace(Shape& shape);
	};
}
