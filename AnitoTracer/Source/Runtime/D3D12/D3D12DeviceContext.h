#pragma once

namespace Anito
{
	class D3D12SwapChain;
	class D3D12DeviceContext
	{
	public:
		D3D12DeviceContext(ID3D12Device10* device);
		~D3D12DeviceContext();
		D3D12DeviceContext(const D3D12DeviceContext&) = delete;
		D3D12DeviceContext& operator=(const D3D12DeviceContext&) = delete;

		void signalAndWaitForGpu(UINT frameIndex);
		void moveToNextFrame(D3D12SwapChain* swapChain);

		ID3D12GraphicsCommandList10* initCommandList(UINT frameIndex, ID3D12PipelineState* pipelineState);
		void executeCommandList();

		void clearRenderTargetColor(D3D12SwapChain* swapChain, float red, float green, float blue, float alpha);
		void setViewportSize(UINT width, UINT height);
		void copyBufferRegion(ID3D12Resource* destination, UINT64 destinationOffset, ID3D12Resource* source, UINT64 sourceOffset, UINT64 numBytes);

		ID3D12Device10* getDevice();
		ID3D12CommandQueue* getCommandQueue();

	private:
		ID3D12Device10* device;

		ID3D12CommandQueue* commandQueue;
		ID3D12CommandAllocator* commandAllocators[2];
		ID3D12GraphicsCommandList10* commandList;

		ID3D12Fence1* fence;
		UINT64 fenceValues[2];
		HANDLE fenceEvent = nullptr;
	};
}

