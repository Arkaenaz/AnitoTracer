#pragma once


namespace Anito
{
	class D3D12Device
	{
	public:
		D3D12Device(IDXGIAdapter4* adapter);
		~D3D12Device();
		D3D12Device(const D3D12Device&) = delete;
		D3D12Device& operator=(const D3D12Device&) = delete;

		ID3D12Device10* get() const;

	private:
		ID3D12Device10* device;
		D3D_FEATURE_LEVEL featureLevel;
	};
}