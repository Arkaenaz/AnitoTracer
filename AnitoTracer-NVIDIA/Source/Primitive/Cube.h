#pragma once
#include "Primitive.h"

class Cube : public DirectXUtil::Primitive
{
public:
	Cube();
	Cube(std::string name);

	Shape createCube(float size, bool uvHorizontalFlip = false, bool uvVerticalFlip = false,
		float uTileFactor = 1, float vTileFactor = 1);
};

