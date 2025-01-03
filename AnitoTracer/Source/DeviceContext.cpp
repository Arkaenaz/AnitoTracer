#include "DeviceContext.h"

#include "Logger.h"

namespace Anito
{
	DeviceContext::DeviceContext(RenderSystem* system, ID3D12Device14* device) : system(system), d3d12device(device)
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		if (Logger::debug(this, this->d3d12device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&this->cmdQueue))))
		{
			Logger::debug(this, "Command Queue created successfully");
		}

		if (Logger::debug(this, this->d3d12device->CreateFence(this->fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence))))
		{
			Logger::debug(this, "Fence created successfully");
		}

		this->fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (this->fenceEvent)
		{
			Logger::debug(this, "Fence Event created successfully");
		}

		if (Logger::debug(this, this->d3d12device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->cmdAllocator))))
		{
			Logger::debug(this, "Command Allocator created successfully");
		}

		if (Logger::debug(this, this->d3d12device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, _uuidof(ID3D12GraphicsCommandList7), (void**)&this->cmdList)))
		{
			Logger::debug(this, "Command List created successfully");
		}
	}

	DeviceContext::~DeviceContext()
	{
		this->cmdList->Release();
		this->cmdAllocator->Release();
		if (this->fenceEvent)
		{
			CloseHandle(this->fenceEvent);
		}
		this->cmdQueue->Release();
	}

	void DeviceContext::signalAndWait()
	{
		this->cmdQueue->Signal(this->fence, ++this->fenceValue);
		if (SUCCEEDED(this->fence->SetEventOnCompletion(this->fenceValue, this->fenceEvent)))
		{
			if (WaitForSingleObject(this->fenceEvent, 20000) != WAIT_OBJECT_0)
			{
				std::exit(-1);
			}
		}
		else
		{
			std::exit(-1);
		}
	}

	ID3D12GraphicsCommandList10* DeviceContext::initCommandList()
	{
		this->cmdAllocator->Reset();
		this->cmdList->Reset(this->cmdAllocator, nullptr);

		return this->cmdList;
	}

	void DeviceContext::executeCommandList()
	{
		if (SUCCEEDED(this->cmdList->Close()))
		{
			ID3D12CommandList* lists[] = { this->cmdList };
			this->cmdQueue->ExecuteCommandLists(1, lists);
			this->signalAndWait();
		}
	}

	ID3D12CommandQueue* DeviceContext::getCommandQueue()
	{
		return this->cmdQueue;
	}
}
