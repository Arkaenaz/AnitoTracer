#include "AnitoTracerPCH.h"
#include "D3D12DeviceContext.h"

#include "D3D12SwapChain.h"

namespace Anito
{
	D3D12DeviceContext::D3D12DeviceContext(ID3D12Device10* device) :
		device(device)
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		if (SUCCEEDED(this->device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&this->commandQueue))))
		{
			Logger::debug(this, "Command Queue created successfully");
		}


		if (SUCCEEDED(this->device->CreateFence(this->fenceValues[0], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence))))
		{
			Logger::debug(this, "Fence created successfully");
		}

		this->fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (this->fenceEvent)
		{
			Logger::debug(this, "Fence Event created successfully");
		}

		for (UINT n = 0; n < D3D12SwapChain::FrameCount; n++)
		{
			if (SUCCEEDED(this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->commandAllocators[n]))))
			{
				Logger::debug(this, "Command Allocator created successfully");
			}
		}

		if (SUCCEEDED(this->device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, _uuidof(ID3D12GraphicsCommandList7), (void**)&this->commandList)))
		{
			Logger::debug(this, "Command List created successfully");
		}
	}

	D3D12DeviceContext::~D3D12DeviceContext()
	{
		this->device->Release();
		this->commandList->Release();
		for (size_t i = 0; i < ARRAYSIZE(this->commandAllocators); i++)
		{
			this->commandAllocators[i]->Release();
		}
		if (this->fenceEvent)
		{
			CloseHandle(this->fenceEvent);
		}
		this->fence->Release();
		this->commandQueue->Release();
	}

	// Wait for pending GPU work to complete.
	void D3D12DeviceContext::signalAndWaitForGpu(UINT frameIndex)
	{
		// Schedule a Signal command in the queue.
		if (FAILED(this->commandQueue->Signal(this->fence, this->fenceValues[frameIndex])))
		{
			// Should Exit or Throw an error
			Logger::throw_exception("Wait for GPU signal failed");
			return;
		}
		// Wait until the fence has been processed.
		if (FAILED(this->fence->SetEventOnCompletion(this->fenceValues[frameIndex], this->fenceEvent)))
		{
			// Should Exit or Throw an error
			Logger::throw_exception("Wait for GPU fence event completion failed");
			return;	
		}

		WaitForSingleObjectEx(this->fenceEvent, INFINITE, false);

		// Increment the fence value for the current frame.
		this->fenceValues[frameIndex]++;
	}

	// Prepare to render the next frame.
	void D3D12DeviceContext::moveToNextFrame(D3D12SwapChain* swapChain)
	{
		const UINT frameIndex = swapChain->getFrameIndex();
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = this->fenceValues[frameIndex];
		if (FAILED(this->commandQueue->Signal(this->fence, currentFenceValue)))
		{
			// Should Exit or Throw an error
			Logger::throw_exception("Move to next frame signal failed");
			return;
		}

		// Update the frame index.
		swapChain->updateFrameIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if (this->fence->GetCompletedValue() < this->fenceValues[frameIndex])
		{
			if(FAILED(this->fence->SetEventOnCompletion(this->fenceValues[frameIndex], this->fenceEvent)))
			{
				// Should Exit or Throw an error
				Logger::throw_exception("Move to next frame fence event completion failed");
				return;
			}
			WaitForSingleObjectEx(this->fenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		this->fenceValues[frameIndex] = currentFenceValue + 1;

		
	}

	ID3D12GraphicsCommandList10* D3D12DeviceContext::initCommandList(UINT frameIndex, ID3D12PipelineState* pipelineState)
	{
		this->commandAllocators[frameIndex]->Reset();
		this->commandList->Reset(this->commandAllocators[frameIndex], pipelineState);

		return this->commandList;
	}

	void D3D12DeviceContext::executeCommandList()
	{
		if (SUCCEEDED(this->commandList->Close()))
		{
			ID3D12CommandList* lists[] = { this->commandList };
			this->commandQueue->ExecuteCommandLists(_countof(lists), lists);
		}
	}

	void D3D12DeviceContext::clearRenderTargetColor(D3D12SwapChain* swapChain, float red, float green, float blue, float alpha)
	{
		UINT frameIndex = swapChain->getFrameIndex();
		UINT rtvDescriptorSize = swapChain->getRTVDescriptorSize();

		FLOAT clearColor[] = { red, green, blue, alpha };

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(swapChain->renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
		this->commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		this->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		/*m_deviceContext->ClearDepthStencilView(renderTexture->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);*/
	}

	void D3D12DeviceContext::setViewportSize(UINT width, UINT height)
	{
		//TODO : Move this into a Graphics Pipeline Class
		D3D12_VIEWPORT vp = {};
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		this->commandList->RSSetViewports(1, &vp);

		RECT scRect;
		scRect.left = scRect.top = 0.0f;
		scRect.right = (FLOAT)width;
		scRect.bottom = (FLOAT)height;
		this->commandList->RSSetScissorRects(1, &scRect);
	}

	void D3D12DeviceContext::copyBufferRegion(ID3D12Resource* destination, UINT64 destinationOffset,
		ID3D12Resource* source, UINT64 sourceOffset, UINT64 numBytes)
	{
		this->commandList->CopyBufferRegion(destination, destinationOffset, source, sourceOffset, numBytes);
	}

	ID3D12Device10* D3D12DeviceContext::getDevice()
	{
		return this->device;
	}

	ID3D12CommandQueue* D3D12DeviceContext::getCommandQueue()
	{
		return this->commandQueue;
	}
}
