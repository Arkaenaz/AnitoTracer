#pragma once

#include "D3D12RenderSystem.h"

namespace Anito
{
	class D3D12RenderSystem;
	class D3D12VertexBuffer
	{
	public:
		D3D12VertexBuffer(D3D12RenderSystem* system);
		~D3D12VertexBuffer();
		D3D12VertexBuffer(const D3D12VertexBuffer&) = delete;
		D3D12VertexBuffer& operator=(const D3D12VertexBuffer&) = delete;

		void load(void* listVertices, UINT sizeVertex, UINT sizeList);
		D3D12_VERTEX_BUFFER_VIEW getVertexBufferView();
	private:
		D3D12RenderSystem* system = nullptr;

		UINT vertexBufferSize;

		ID3D12Resource2* vertexBuffer;
		//ID3D12Resource2* uploadBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	};
}
