#pragma once
#include "Runtime/D3D12/D3D12Buffer.h"

namespace Anito
{
	class D3D12IndexBuffer
	{
	public:
		D3D12IndexBuffer();
		~D3D12IndexBuffer();
		D3D12IndexBuffer(const D3D12IndexBuffer&) = delete;
		D3D12IndexBuffer& operator=(const D3D12IndexBuffer&) = delete;

		void load(const D3D12Device& device, void* listVertices, UINT sizeList);
		D3D12_INDEX_BUFFER_VIEW getIndexBufferView();

	private:
		UINT indexBufferSize;
		D3D12Buffer* indexBuffer;
		//D3D12Buffer* uploadBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
	};
}
