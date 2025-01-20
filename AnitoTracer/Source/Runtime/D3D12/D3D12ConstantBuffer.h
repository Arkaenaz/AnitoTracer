#pragma once

#include "D3D12Buffer.h"

namespace Anito
{
	class D3D12ConstantBuffer
	{
	public:
		D3D12ConstantBuffer();
		~D3D12ConstantBuffer();
		D3D12ConstantBuffer(const D3D12ConstantBuffer&) = delete;
		D3D12ConstantBuffer& operator=(const D3D12ConstantBuffer&) = delete;

		void load(const D3D12Device& device, void* buffer, UINT sizeBuffer);
		D3D12_CONSTANT_BUFFER_VIEW_DESC getIndexBufferView();
		//void update(DeviceContextPtr context, void* buffer);

	private:
		UINT constantBufferSize;

		D3D12Buffer* constantBuffer;
		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferView;
	};
}