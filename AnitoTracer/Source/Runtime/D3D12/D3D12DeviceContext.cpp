#include "AnitoTracerPCH.h"
#include "D3D12DeviceContext.h"

namespace Anito
{
	D3D12DeviceContext::D3D12DeviceContext(D3D12RenderSystem* system, ID3D12Device10* device) : system(system)
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		if (SUCCEEDED(device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&this->cmdQueue))))
		{
			Logger::debug(this, "Command Queue created successfully");
		}


		if (SUCCEEDED(device->CreateFence(this->fenceValues[0], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence))))
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
			if (SUCCEEDED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->cmdAllocators[n]))))
			{
				Logger::debug(this, "Command Allocator created successfully");
			}
		}

		if (SUCCEEDED(device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, _uuidof(ID3D12GraphicsCommandList7), (void**)&this->cmdList)))
		{
			Logger::debug(this, "Command List created successfully");
		}
	}

	D3D12DeviceContext::~D3D12DeviceContext()
	{
		this->cmdList->Release();
		this->cmdAllocators[1]->Release();
		this->cmdAllocators[0]->Release();
		if (this->fenceEvent)
		{
			CloseHandle(this->fenceEvent);
		}
		this->fence->Release();
		this->cmdQueue->Release();
	}

	// Wait for pending GPU work to complete.
	void D3D12DeviceContext::signalAndWaitForGpu(UINT frameIndex)
	{
		// Schedule a Signal command in the queue.
		if (FAILED(this->cmdQueue->Signal(this->fence, this->fenceValues[frameIndex])))
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
		if (FAILED(this->cmdQueue->Signal(this->fence, currentFenceValue)))
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

	ID3D12GraphicsCommandList10* D3D12DeviceContext::initCommandList(UINT frameIndex)
	{
		this->cmdAllocators[frameIndex]->Reset();
		this->cmdList->Reset(this->cmdAllocators[frameIndex], nullptr);

		return this->cmdList;
	}

	void D3D12DeviceContext::executeCommandList()
	{
		if (SUCCEEDED(this->cmdList->Close()))
		{
			ID3D12CommandList* lists[] = { this->cmdList };
			this->cmdQueue->ExecuteCommandLists(_countof(lists), lists);
		}
	}

	void D3D12DeviceContext::clearRenderTargetColor(D3D12SwapChain* swapChain, float red, float green, float blue, float alpha)
	{
		UINT frameIndex = swapChain->getFrameIndex();
		UINT rtvDescriptorSize = swapChain->getRTVDescriptorSize();

		FLOAT clearColor[] = { red, green, blue, alpha };

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(swapChain->renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
		this->cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		this->cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		/*m_deviceContext->ClearDepthStencilView(renderTexture->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);*/
	}

	void D3D12DeviceContext::setViewportSize(UINT width, UINT height)
	{
		D3D12_VIEWPORT vp = {};
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		this->cmdList->RSSetViewports(1, &vp);

		RECT scRect;
		scRect.left = scRect.top = 0.0f;
		scRect.right = (FLOAT)width;
		scRect.bottom = (FLOAT)height;
		this->cmdList->RSSetScissorRects(1, &scRect);
	}

	ID3D12CommandQueue* D3D12DeviceContext::getCommandQueue()
	{
		return this->cmdQueue;
	}
}
