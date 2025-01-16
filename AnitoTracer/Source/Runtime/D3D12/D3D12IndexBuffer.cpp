#include "AnitoTracerPCH.h"
#include "D3D12IndexBuffer.h"

namespace Anito
{
	D3D12IndexBuffer::D3D12IndexBuffer()
	{
	}

	D3D12IndexBuffer::~D3D12IndexBuffer()
	{
	}

	void D3D12IndexBuffer::load(const D3D12Device& device, void* listVertices, UINT sizeList)
	{
		this->indexBufferSize = sizeof(unsigned int) * sizeList;

		this->indexBuffer = D3D12Buffer::createIndexBuffer(device, this->indexBufferSize);
		this->indexBuffer->get()->SetName(L"Index Buffer");
		// TODO : Readd upload buffer when you know how to make it and when you know how to copy over buffers

		//this->uploadBuffer = D3D12Buffer::createUploadBuffer(*device, this->vertexBufferSize);

		this->indexBuffer->copyFromCPU(listVertices, this->indexBufferSize);

		this->indexBufferView.BufferLocation = this->indexBuffer->getGPUVirtualAddress();
		this->indexBufferView.SizeInBytes = this->indexBufferSize;
		this->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	D3D12_INDEX_BUFFER_VIEW D3D12IndexBuffer::getIndexBufferView()
	{
		return this->indexBufferView;
	}
}
