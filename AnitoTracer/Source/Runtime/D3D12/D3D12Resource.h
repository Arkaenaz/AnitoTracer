#pragma once

namespace Anito
{
	class D3D12Resource
	{
	public:
		explicit D3D12Resource(ID3D12Resource2* resource);
		virtual ~D3D12Resource();
		D3D12Resource() = delete;
		D3D12Resource(const D3D12Resource&) = delete;
		D3D12Resource& operator=(const D3D12Resource&) = delete;

		ID3D12Resource2* get() const;
		D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress();

	protected:

		ID3D12Resource2* resource;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress {};
	};
}