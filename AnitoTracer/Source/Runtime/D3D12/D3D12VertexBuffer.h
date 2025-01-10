#pragma once

#include "D3D12Buffer.h"
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

		void load(const D3D12Device& device, void* listVertices, UINT sizeVertex, UINT sizeList);
		D3D12_VERTEX_BUFFER_VIEW getVertexBufferView();

	private:
		D3D12RenderSystem* system = nullptr;

		UINT vertexBufferSize;

		D3D12Buffer* vertexBuffer;
		//D3D12Buffer* uploadBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	};
}
