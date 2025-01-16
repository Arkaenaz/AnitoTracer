#pragma once

#include "D3D12Resource.h"

#include "D3D12CommandContext.h"
#include "D3D12Device.h"

namespace Anito
{
	class D3D12Resource;
	class D3D12Buffer : public D3D12Resource
	{
	public:
		static D3D12Buffer* createVertexBuffer(const D3D12Device& device, UINT size);
		static D3D12Buffer* createIndexBuffer(const D3D12Device& device, UINT size);
		//static D3D12Buffer* createUploadBuffer(const D3D12Device& device, UINT size);

		D3D12Buffer(ID3D12Resource2* resource);
		~D3D12Buffer() override;
		D3D12Buffer(const D3D12Buffer&) = delete;
		D3D12Buffer& operator=(const D3D12Buffer&) = delete;

		void copyFromCPU(const void* data, UINT size) const;
		void copyFromBuffer(D3D12CommandContext* context, const D3D12Buffer& buffer) const;
	};
}


