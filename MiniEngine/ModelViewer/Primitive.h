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
#include "Renderer.h"

#include "Model.h"

using namespace Math;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

__declspec(align(256)) struct Constants
{
	Matrix4 World;         // Object to world
	Matrix3 WorldIT;         // Object to world
};

class Primitive
{
public:
	enum ObjectType { CUBE, SPHERE, PLANE, CYLINDER, CAPSULE };

	Primitive(std::string name, ObjectType type);
	~Primitive();

	virtual void CreateType(ObjectType type);
	virtual void Update(float deltaTime, RECT viewport);
	virtual void Render(GraphicsContext& context, Matrix4 viewMat);
	virtual void Render(Renderer::MeshSorter& sorter,
		const GpuBuffer& Constants,
		const Math::AffineTransform sphereTransforms[]) const;

	GraphicsPSO graphicsPSO;
	StructuredBuffer vertexBuffer;
	ByteAddressBuffer indexBuffer;
	ID3D10Blob* vertexShader;
	ID3D10Blob* pixelShader;

	ObjectType type;
	std::string name;

	bool rendered = false;

	BoundingSphere m_BoundingSphere; // Object-space bounding sphere
	AxisAlignedBox m_BoundingBox;
	ByteAddressBuffer m_DataBuffer;
	uint32_t m_NumNodes;
	uint32_t m_NumMeshes;
	std::unique_ptr<uint8_t[]> m_MeshData;
	std::unique_ptr<GraphNode[]> m_SceneGraph;

protected:
	void CreateCube();
	void CreateSphere();
	void CreatePlane();
	void CreateCylinder();
	void CreateCapsule();
};

class PrimitiveInstance
{
public:
	PrimitiveInstance() {}
	~PrimitiveInstance() {
		m_ConstantsCPU.Destroy();
		m_ConstantsGPU.Destroy();
	}
	PrimitiveInstance(std::shared_ptr<const Primitive> sourcePrimitive);
	PrimitiveInstance(const PrimitiveInstance& PrimitiveInstance);

	PrimitiveInstance& operator=(std::shared_ptr<const Primitive> sourcePrimitive);

	bool IsNull(void) const { return m_Primitive == nullptr; }

	void Update(GraphicsContext& gfxContext, float deltaTime);
	void Render(Renderer::MeshSorter& sorter) const;

	void Resize(float newRadius);
	Vector3 GetCenter() const;
	Scalar GetRadius() const;
	BoundingSphere GetBoundingSphere() const;
	OrientedBox GetBoundingBox() const;

private:
	std::shared_ptr<const Primitive> m_Primitive;
	UploadBuffer m_ConstantsCPU;
	ByteAddressBuffer m_ConstantsGPU;
	std::unique_ptr<Math::AffineTransform[]> m_BoundingSphereTransforms;
	UniformTransform m_Locator;
};