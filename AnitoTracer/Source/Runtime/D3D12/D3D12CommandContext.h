#pragma once

#include "D3D12CommandQueue.h"
#include "D3D12Device.h"
#include "D3D12Resource.h"

namespace Anito
{
	class D3D12SwapChain;
	class D3D12CommandContext
	{
	public:
		D3D12CommandContext(const D3D12Device& device);
		~D3D12CommandContext();
		D3D12CommandContext(const D3D12CommandContext&) = delete;
		D3D12CommandContext& operator=(const D3D12CommandContext&) = delete;

		void signalAndWaitForGpu(UINT frameIndex);
		void beginFrame(D3D12Resource* renderTarget);
		void endFrame(D3D12Resource* renderTarget);
		void moveToNextFrame(D3D12SwapChain* swapChain);

		ID3D12GraphicsCommandList10* initCommandList(UINT frameIndex, ID3D12PipelineState* pipelineState);
		void executeCommandList();

		void clearRenderTargetColor(D3D12SwapChain* swapChain, float red, float green, float blue, float alpha);
		void setViewportandScissor(UINT x, UINT y, UINT width, UINT height);
		void setViewport(FLOAT x, FLOAT y, FLOAT width, FLOAT height);
		void setScissor(LONG x, LONG y, LONG width, LONG height);
		void copyBufferRegion(ID3D12Resource* destination, UINT64 destinationOffset, ID3D12Resource* source, UINT64 sourceOffset, UINT64 numBytes);

		void resetFenceValues();

		D3D12CommandQueue* getCommandQueue();

	private:
		void signal(UINT64 fenceValue);
		void waitForFenceValue(UINT frameIndex, UINT64 fenceValue);

		D3D12CommandQueue* commandQueue;
		ID3D12CommandAllocator* commandAllocators[2];
		ID3D12GraphicsCommandList10* commandList;

		ID3D12Fence1* fence;
		UINT64 currentFenceValue = 0;
		UINT64 fenceValues[2];
		HANDLE fenceEvent = nullptr;
	};
}

