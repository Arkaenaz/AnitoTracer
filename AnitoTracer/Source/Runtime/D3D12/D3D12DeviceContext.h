#pragma once

#include "D3D12RenderSystem.h"

namespace Anito
{
	class D3D12RenderSystem;
	class D3D12SwapChain;
	class D3D12DeviceContext
	{
	public:
		D3D12DeviceContext(D3D12RenderSystem* system, ID3D12Device14* device);
		~D3D12DeviceContext();

		void signalAndWait();

		ID3D12GraphicsCommandList10* initCommandList();
		void executeCommandList();

		void clearRenderTargetColor(const D3D12SwapChain* swapChain, float red, float green, float blue, float alpha);

		ID3D12CommandQueue* getCommandQueue();

	private:
		D3D12RenderSystem* system;
		ID3D12Device14* d3d12device;
		ID3D12CommandQueue* cmdQueue;

		ID3D12CommandAllocator* cmdAllocator;
		ID3D12GraphicsCommandList10* cmdList;

		ID3D12Fence1* fence;
		UINT64 fenceValue = 0;
		HANDLE fenceEvent = nullptr;
	};
}

