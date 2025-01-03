#pragma once

#include "WinInclude.h"

#include "DeviceContext.h"
#include "SwapChain.h"

namespace Anito
{
	class DeviceContext;
	class SwapChain;
	class RenderSystem
	{
	public:
		SwapChain* createSwapChain(HWND hwnd, UINT width, UINT height);
		DeviceContext* getImmediateDeviceContext();
		IDXGIFactory7* getDXFactory();
		ID3D12Device14* getDXDevice();

	private:
		friend class GraphicsEngine;
		friend class SwapChain;

		RenderSystem();
		~RenderSystem();

		DeviceContext* immediateDeviceContext;

		ID3D12Device14* d3d12Device;
		D3D_FEATURE_LEVEL featureLevel;

		//IDXGIDevice4* dxgiDevice;
		IDXGIAdapter4* dxgiAdapter;
		IDXGIFactory7* dxgiFactory;
	};
}