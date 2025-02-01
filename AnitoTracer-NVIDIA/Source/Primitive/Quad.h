#pragma once
#include "Primitive.h"

class Quad : public DirectXUtil::Primitive
{
public:
	Quad();
	Quad(std::string name);

	Shape createQuad(int size, bool uvHorizontalFlip = false, bool uvVerticalFlip = false,
		float uTileFactor = 1, float vTileFactor = 1);
};

