#pragma once

#include "D3D12RenderSystem.h"

namespace Anito
{
	class D3D12RenderSystem;
	class D3D12SwapChain
	{
	public:
		D3D12SwapChain(D3D12RenderSystem* system, HWND hwnd, UINT width, UINT height);
		~D3D12SwapChain();

		void cleanRenderTarget();
		void resizeBuffers(UINT width, UINT height);
		void createRenderTarget();
		bool present(bool vsync);

		void updateFrameIndex();
		UINT getFrameIndex();
		UINT getRTVDescriptorSize();
		ID3D12Resource2* getRenderTarget(UINT frameIndex);

	private:
		static const UINT FrameCount = 2;

		friend class D3D12DeviceContext;

		D3D12RenderSystem* system = nullptr;

		UINT frameIndex;
		IDXGISwapChain4* swapChain;
		ID3D12DescriptorHeap* renderTargetViewHeap;
		UINT rtvDescriptorSize;
		ID3D12Resource2* renderTargets[FrameCount];
	};
}