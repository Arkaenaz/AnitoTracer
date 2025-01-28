#pragma once
#include <string>
#include "BufferManager.h"
#include "RootSignature.h"

using namespace Math;

class Primitive
{
public:
	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	enum ObjectType { CUBE };

protected:
	GraphicsPSO graphicsPSO;
	RootSignature* rootSignature;

	StructuredBuffer vertexBuffer;
	ByteAddressBuffer indexBuffer;

	ID3D10Blob* vertexShader;
	ID3D10Blob* pixelShader;

	ObjectType type;
	std::string name;

public:
	Primitive(std::string name, ObjectType type, RootSignature* rs);
	~Primitive();

	virtual void init(ObjectType type);
	void createCube();

	virtual void update(float deltaTime, RECT viewport);
	virtual void draw(GraphicsContext& context, Matrix4 viewMat);
};

