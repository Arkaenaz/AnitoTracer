#pragma once

#include "RenderSystem.h"

namespace Anito
{
	class RenderSystem;
	class SwapChain
	{
	private:
		RenderSystem* system = nullptr;

		IDXGISwapChain4* swapChain;
		ID3D12DescriptorHeap* renderTargetViewHeap;
		UINT rtvDescriptorSize;

	private:
		friend class DeviceContext;

	public:
		SwapChain(RenderSystem* system, HWND hwnd, UINT width, UINT height);
		~SwapChain();

	public:
		void cleanRenderTarget();
		void resizeBuffers(UINT bufferCount, UINT width, UINT height);
		void createRenderTarget();
		bool present(bool vsync);
	};
}