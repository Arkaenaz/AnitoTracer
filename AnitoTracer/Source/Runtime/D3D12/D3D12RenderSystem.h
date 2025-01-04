#pragma once

#include "D3D12DeviceContext.h"
#include "D3D12SwapChain.h"

namespace Anito
{
	class D3D12DeviceContext;
	class D3D12SwapChain;
	class D3D12RenderSystem
	{
	public:
		D3D12SwapChain* createSwapChain(HWND hwnd, UINT width, UINT height);
		D3D12DeviceContext* getImmediateDeviceContext();
		IDXGIFactory7* getDXFactory();
		ID3D12Device10* getDXDevice();

	private:
		friend class D3D12SwapChain;

		D3D12DeviceContext* immediateDeviceContext;

		ID3D12Device10* device;
		D3D_FEATURE_LEVEL featureLevel;

		//IDXGIDevice4* dxgiDevice;
		IDXGIAdapter4* dxgiAdapter;
		IDXGIFactory7* dxgiFactory;

	public:
		static D3D12RenderSystem* getInstance();
		static bool initialize();
		static void destroy();

	private:
		static D3D12RenderSystem* P_SHARED_INSTANCE;

		D3D12RenderSystem();
		~D3D12RenderSystem();
		D3D12RenderSystem(D3D12RenderSystem const&);
		D3D12RenderSystem& operator = (D3D12RenderSystem const&);
	};
}