#include "AnitoTracerPCH.h"
#include "D3D12ConstantBuffer.h"

#include "D3D12RenderSystem.h"
//#include "D3D12DeviceContext.h"

namespace Anito
{
	D3D12ConstantBuffer::D3D12ConstantBuffer()
	{
	}

	D3D12ConstantBuffer::~D3D12ConstantBuffer()
	{
		delete this->constantBuffer;
	}

	void D3D12ConstantBuffer::load(const D3D12Device& device, void* buffer, UINT sizeBuffer)
	{
		this->constantBufferSize = sizeBuffer;

		this->constantBuffer = D3D12Buffer::createConstantBuffer(device, this->constantBufferSize);
		this->constantBuffer->get()->SetName(L"Constant Buffer");

		this->constantBuffer->copyFromCPU(buffer, this->constantBufferSize);

		this->constantBufferView.BufferLocation = this->constantBuffer->getGPUVirtualAddress();
		this->constantBufferView.SizeInBytes = this->constantBufferSize;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC D3D12ConstantBuffer::getIndexBufferView()
	{
		return this->constantBufferView;
	}

	/*void D3D12ConstantBuffer::update(DeviceContextPtr context, void* buffer)
	{
		context->devContext->UpdateSubresource(this->m_buffer, NULL, NULL, buffer, NULL, NULL);
	}*/
}