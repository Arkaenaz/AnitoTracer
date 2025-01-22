#pragma once
#include <iostream>
#include <string>

#include "../../Runtime/D3D12/D3D12VertexBuffer.h"
#include "../../Runtime/D3D12/D3D12ConstantBuffer.h"

#include "../GameObject.h"

namespace Anito
{
	class Quad : public GameObject
	{
	public:
		D3D12VertexBuffer* vb;
		D3D12ConstantBuffer* cb;

		ID3D10Blob* vertexShader;
		ID3D10Blob* pixelShader;
		/*VertexShaderPtr vs;
		PixelShaderPtr ps;*/

		float height;
		float width;

	public:
		Quad(std::string name, Vector2 size);
		~Quad();

		void update(float deltaTime) override;
		void draw() override;
		void render(ID3D12GraphicsCommandList10* cmdList);
		bool release();
	};
}
