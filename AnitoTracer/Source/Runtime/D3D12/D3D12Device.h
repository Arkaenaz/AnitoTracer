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

/*#if defined(NTDDI_WIN11_ZN)
		ID3D12Device10* get() const;
#else*/
		ID3D12Device9* get() const;
//#endif

	private:
/*#if defined(NTDDI_WIN11_ZN)
		ID3D12Device10* device;
#else*/
		ID3D12Device9* device;
//#endif
#ifdef _DEBUG
		ID3D12InfoQueue* infoQueue;
#endif
		D3D_FEATURE_LEVEL featureLevel;
	};
}