#include "AnitoTracerPCH.h"
#include "D3D12CommandQueue.h"

namespace Anito {
	D3D12CommandQueue::D3D12CommandQueue(const D3D12Device& device)
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		if (SUCCEEDED(device.get()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&this->commandQueue))))
		{
			Logger::debug(this, "Command Queue created successfully");
		}
	}

	D3D12CommandQueue::~D3D12CommandQueue()
	{
		this->commandQueue->Release();
	}

	void D3D12CommandQueue::execute(const std::vector<ID3D12CommandList*>& commandLists) const
	{
		this->commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
	}

	ID3D12CommandQueue* D3D12CommandQueue::get() const
	{
		return this->commandQueue;
	}
}