#pragma once
#include "PrimitivesInclude.h"

namespace Anito
{
	class Primitive : public GameObject
	{
	protected:
		D3D12VertexBuffer* vb;
		D3D12IndexBuffer* ib;

		/*VertexShaderPtr vs;
		PixelShaderPtr ps;*/

	public:
		Primitive(std::string name, ObjectType type);
		~Primitive();

		virtual void init();
		virtual void initializeBuffers() = 0;
		virtual void createVertexShader();
		virtual void createPixelShader();
		virtual void createConstantBuffer();

		virtual std::vector<Vertex3D> createVertices() = 0;

		virtual void update(float deltaTime) override;
		virtual void draw() override;
	};
}
