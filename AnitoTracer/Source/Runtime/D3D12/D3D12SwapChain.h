#pragma once

#include "D3D12RenderSystem.h"

namespace Anito
{
	class D3D12RenderSystem;
	class D3D12SwapChain
	{
	public:
		D3D12SwapChain(const D3D12Device& device, D3D12RenderSystem* system, UINT width, UINT height, HWND handle);
		~D3D12SwapChain();
		D3D12SwapChain(const D3D12SwapChain&) = delete;
		D3D12SwapChain& operator=(const D3D12SwapChain&) = delete;

		void cleanRenderTarget();
		void resizeBuffers(UINT width, UINT height);
		void createRenderTarget();
		bool present(bool vsync);

		void updateFrameIndex();
		UINT getFrameIndex();
		UINT getRTVDescriptorSize();
		D3D12Resource* getRenderTarget(UINT frameIndex);
		D3D12Resource* getCurrentRenderTarget();
		CD3DX12_CPU_DESCRIPTOR_HANDLE getRenderTargetViewHandle();

	private:
		const D3D12Device& device;

		static const UINT FrameCount = 2;

		friend class D3D12CommandContext;

		UINT frameIndex;
		IDXGISwapChain4* swapChain;
		ID3D12DescriptorHeap* renderTargetViewHeap;
		UINT rtvDescriptorSize;
		D3D12Resource* renderTargets[FrameCount];
		UINT swapChainFlags = 0;
	};
}