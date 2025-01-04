#include "AnitoTracerPCH.h"
#include "D3D12DeviceContext.h"

namespace Anito
{
	D3D12DeviceContext::D3D12DeviceContext(D3D12RenderSystem* system, ID3D12Device10* device) : system(system), d3d12device(device)
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		if (SUCCEEDED(this->d3d12device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&this->cmdQueue))))
		{
			Logger::debug(this, "Command Queue created successfully");
		}


		if (SUCCEEDED(this->d3d12device->CreateFence(this->fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence))))
		{
			Logger::debug(this, "Fence created successfully");
		}

		this->fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (this->fenceEvent)
		{
			Logger::debug(this, "Fence Event created successfully");
		}

		if (SUCCEEDED(this->d3d12device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->cmdAllocator))))
		{
			Logger::debug(this, "Command Allocator created successfully");
		}

		if (SUCCEEDED(this->d3d12device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, _uuidof(ID3D12GraphicsCommandList7), (void**)&this->cmdList)))
		{
			Logger::debug(this, "Command List created successfully");
		}
	}

	D3D12DeviceContext::~D3D12DeviceContext()
	{
		this->cmdList->Release();
		this->cmdAllocator->Release();
		if (this->fenceEvent)
		{
			CloseHandle(this->fenceEvent);
		}
		this->cmdQueue->Release();
	}

	void D3D12DeviceContext::signalAndWait()
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

	ID3D12GraphicsCommandList10* D3D12DeviceContext::initCommandList()
	{
		this->cmdAllocator->Reset();
		this->cmdList->Reset(this->cmdAllocator, nullptr);

		return this->cmdList;
	}

	void D3D12DeviceContext::executeCommandList()
	{
		if (SUCCEEDED(this->cmdList->Close()))
		{
			ID3D12CommandList* lists[] = { this->cmdList };
			this->cmdQueue->ExecuteCommandLists(_countof(lists), lists);
			this->signalAndWait();
		}
	}

	void D3D12DeviceContext::clearRenderTargetColor(const D3D12SwapChain* swapChain, float red, float green, float blue, float alpha)
	{
		FLOAT clearColor[] = { red, green, blue, alpha };
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain->renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
		this->cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		//m_deviceContext->ClearDepthStencilView(renderTexture->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		this->cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	}

	ID3D12CommandQueue* D3D12DeviceContext::getCommandQueue()
	{
		return this->cmdQueue;
	}
}
