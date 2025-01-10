#pragma once

namespace Anito
{
	class D3D12Resource
	{
	public:
		virtual ~D3D12Resource();
		D3D12Resource(const D3D12Resource&) = delete;
		D3D12Resource& operator=(const D3D12Resource&) = delete;

		ID3D12Resource2* get() const;

	protected:
		D3D12Resource(ID3D12Resource2* resource);

		ID3D12Resource2* resource;
	};
}