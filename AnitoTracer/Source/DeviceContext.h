#pragma once

#include "RenderSystem.h"

namespace Anito
{
	class RenderSystem;
	class DeviceContext
	{
	public:
		DeviceContext(RenderSystem* system, ID3D12Device14* device);
		~DeviceContext();

		void signalAndWait();

		ID3D12GraphicsCommandList10* initCommandList();
		void executeCommandList();

		ID3D12CommandQueue* getCommandQueue();

	private:
		RenderSystem* system;
		ID3D12Device14* d3d12device;
		ID3D12CommandQueue* cmdQueue;

		ID3D12CommandAllocator* cmdAllocator;
		ID3D12GraphicsCommandList10* cmdList;

		ID3D12Fence1* fence;
		UINT64 fenceValue = 0;
		HANDLE fenceEvent = nullptr;
	};
}

