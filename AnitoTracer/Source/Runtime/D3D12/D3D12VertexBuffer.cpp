#include "AnitoTracerPCH.h"
#include "D3D12VertexBuffer.h"

namespace Anito
{
	D3D12VertexBuffer::D3D12VertexBuffer()
	{
	}

	D3D12VertexBuffer::~D3D12VertexBuffer()
	{
        delete this->vertexBuffer;
	}

	void D3D12VertexBuffer::load(const D3D12Device& device, void* listVertices, UINT sizeVertex, UINT sizeList)
	{
        this->vertexBufferSize = sizeVertex * sizeList;

		this->vertexBuffer = D3D12Buffer::createVertexBuffer(device, this->vertexBufferSize);

        // TODO : Readd upload buffer when you know how to make it and when you know how to copy over buffers

		//this->uploadBuffer = D3D12Buffer::createUploadBuffer(*device, this->vertexBufferSize);

		this->vertexBuffer->copyFromCPU(listVertices, this->vertexBufferSize);

        this->vertexBufferView.BufferLocation = this->vertexBuffer->get()->GetGPUVirtualAddress();
        this->vertexBufferView.StrideInBytes = sizeVertex;
        this->vertexBufferView.SizeInBytes = this->vertexBufferSize;
	}

	D3D12_VERTEX_BUFFER_VIEW D3D12VertexBuffer::getVertexBufferView()
	{
        return this->vertexBufferView;
	}
}

