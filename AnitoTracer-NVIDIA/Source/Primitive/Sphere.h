#pragma once
#include "Primitive.h"

class Sphere : public DirectXUtil::Primitive
{
public:
	Sphere();
	Sphere(std::string name);

	Shape createSphere(float diameter, int tessellation, bool uvHorizontalFlip = false,
		bool uvVerticalFlip = false);
};

