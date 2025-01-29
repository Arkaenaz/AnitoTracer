//
// AnitoTracer - Ray Tracing Renderer
// Developed by 

#pragma once

#include "BufferManager.h"

// Testing
#include "GpuBuffer.h"
#include "VectorMath.h"
#include "Camera.h"
#include "UploadBuffer.h"
#include "TextureManager.h"
#include "Math/BoundingBox.h"
#include "Math/BoundingSphere.h"
#include <cstdint>

using namespace Math;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class Primitive
{
public:
	enum ObjectType { CUBE, SPHERE, PLANE, CYLINDER, CAPSULE };

protected:
	GraphicsPSO graphicsPSO;
	StructuredBuffer vertexBuffer;
	ByteAddressBuffer indexBuffer;
	ID3D10Blob* vertexShader;
	ID3D10Blob* pixelShader;

	ObjectType type;
	std::string name;

	bool rendered = false;

	Math::BoundingSphere m_BoundingSphere; // Object-space bounding sphere
	Math::AxisAlignedBox m_BoundingBox;
	ByteAddressBuffer m_DataBuffer;
	ByteAddressBuffer m_MaterialConstants;
	uint32_t m_NumNodes;
	uint32_t m_NumMeshes;
	uint32_t m_NumAnimations;
	uint32_t m_NumJoints;
	std::unique_ptr<uint8_t[]> m_MeshData;
	std::vector<TextureRef> textures;
	std::unique_ptr<uint16_t[]> m_JointIndices;
	std::unique_ptr<Math::Matrix4[]> m_JointIBMs;

public:
	Primitive(std::string name, ObjectType type);
	~Primitive();

	virtual void CreateType(ObjectType type);
	void CreateCube();
	void CreateSphere();
	void CreatePlane();
	void CreateCylinder();
	void CreateCapsule();

	virtual void update(float deltaTime, RECT viewport);
	virtual void draw(GraphicsContext& context, Matrix4 viewMat);
};

