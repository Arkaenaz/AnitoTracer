#pragma once

#include "D3D12RenderSystem.h"

namespace Anito
{
	class D3D12RenderSystem;
	class D3D12SwapChain;
	class D3D12DeviceContext
	{
	public:
		D3D12DeviceContext(D3D12RenderSystem* system, ID3D12Device10* device);
		~D3D12DeviceContext();

		void signalAndWaitForGpu(UINT frameIndex);
		void moveToNextFrame(D3D12SwapChain* swapChain);

		ID3D12GraphicsCommandList10* initCommandList(UINT frameIndex);
		void executeCommandList();

		void clearRenderTargetColor(D3D12SwapChain* swapChain, float red, float green, float blue, float alpha);
		void setViewportSize(UINT width, UINT height);

		ID3D12CommandQueue* getCommandQueue();

	private:
		D3D12RenderSystem* system;
		ID3D12CommandQueue* cmdQueue;

		ID3D12CommandAllocator* cmdAllocators[2];
		ID3D12GraphicsCommandList10* cmdList;

		ID3D12Fence1* fence;
		UINT64 fenceValues[2];
		HANDLE fenceEvent = nullptr;
	};
}

