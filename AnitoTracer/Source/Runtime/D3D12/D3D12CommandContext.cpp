#include "AnitoTracerPCH.h"
#include "D3D12CommandContext.h"

#include "D3D12SwapChain.h"
#include "D3D12CommandQueue.h"

namespace Anito
{
	D3D12CommandContext::D3D12CommandContext(const D3D12Device& device)
	{
		this->commandQueue = new D3D12CommandQueue(device);

		if (SUCCEEDED(device.get()->CreateFence(this->currentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence))))
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
			if (SUCCEEDED(device.get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->commandAllocators[n]))))
			{
				Logger::debug(this, "Command Allocator created successfully");
			}
		}

		if (SUCCEEDED(device.get()->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, _uuidof(ID3D12GraphicsCommandList7), (void**)&this->commandList)))
		{
			Logger::debug(this, "Command List created successfully");
		}
	}

	D3D12CommandContext::~D3D12CommandContext()
	{
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
		delete this->commandQueue;
	}

	// Wait for pending GPU work to complete.
	void D3D12CommandContext::signalAndWaitForGpu(UINT frameIndex)
	{
		// Schedule a Signal command in the queue.
		signal(this->currentFenceValue);

		// Wait until the fence has been processed.
		this->waitForFenceValue(frameIndex, this->currentFenceValue);

		// Increment the fence value for the current frame.
		this->currentFenceValue++;
		this->fenceValues[frameIndex] = this->currentFenceValue++;
	}

	void D3D12CommandContext::beginFrame(D3D12Resource* renderTarget)
	{
		// Indicate that the back buffer will be used as a render target.
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		this->commandList->ResourceBarrier(1, &barrier);
	}

	void D3D12CommandContext::endFrame(D3D12Resource* renderTarget)
	{
		// Indicate that the back buffer will now be used to present.
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		this->commandList->ResourceBarrier(1, &barrier);
	}

	// TODO: Let me know if I can do delegates :3
	// Prepare to render the next frame.
	void D3D12CommandContext::moveToNextFrame(D3D12SwapChain* swapChain)
	{
		UINT frameIndex = swapChain->getFrameIndex();
		// Schedule a Signal command in the queue.
		signal(this->currentFenceValue);

		// Update the frame index, so we wait if the next frame is ready.
		swapChain->updateFrameIndex();
		frameIndex = swapChain->getFrameIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		this->waitForFenceValue(frameIndex, this->currentFenceValue);

		// Set the fence value for the next frame.
		this->currentFenceValue++;
		this->fenceValues[frameIndex] = this->currentFenceValue;
	}

	ID3D12GraphicsCommandList10* D3D12CommandContext::initCommandList(UINT frameIndex, ID3D12PipelineState* pipelineState)
	{
		this->commandAllocators[frameIndex]->Reset();
		this->commandList->Reset(this->commandAllocators[frameIndex], pipelineState);

		return this->commandList;
	}

	void D3D12CommandContext::executeCommandList()
	{
		if (SUCCEEDED(this->commandList->Close()))
		{
			std::vector<ID3D12CommandList*> lists = { this->commandList };
			this->commandQueue->execute(lists);
		}
	}

	void D3D12CommandContext::clearRenderTargetColor(D3D12SwapChain* swapChain, float red, float green, float blue, float alpha)
	{
		UINT frameIndex = swapChain->getFrameIndex();
		UINT rtvDescriptorSize = swapChain->getRTVDescriptorSize();

		FLOAT clearColor[] = { red, green, blue, alpha };

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(swapChain->renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
		this->commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		this->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		/*m_deviceContext->ClearDepthStencilView(renderTexture->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);*/
	}

	void D3D12CommandContext::setViewportSize(FLOAT width, FLOAT height)
	{
		//TODO : Move this into a Graphics Pipeline Class
		D3D12_VIEWPORT vp = {};
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		this->commandList->RSSetViewports(1, &vp);

		RECT scRect;
		scRect.left = scRect.top = 0l;
		scRect.right = static_cast<LONG>(width);
		scRect.bottom = static_cast<LONG>(height);
		this->commandList->RSSetScissorRects(1, &scRect);
	}

	void D3D12CommandContext::copyBufferRegion(ID3D12Resource* destination, UINT64 destinationOffset,
		ID3D12Resource* source, UINT64 sourceOffset, UINT64 numBytes)
	{
		this->commandList->CopyBufferRegion(destination, destinationOffset, source, sourceOffset, numBytes);
	}

	D3D12CommandQueue* D3D12CommandContext::getCommandQueue()
	{
		return this->commandQueue;
	}

	void D3D12CommandContext::signal(UINT64 fenceValue)
	{
		if (FAILED(this->commandQueue->get()->Signal(this->fence, fenceValue)))
		{
			// Should Exit or Throw an error
			Logger::throw_exception("Signal failed.");
		}
	}

	void D3D12CommandContext::waitForFenceValue(UINT frameIndex, UINT64 fenceValue)
	{
		if (this->fence->GetCompletedValue() < this->fenceValues[frameIndex])
		{
			if (FAILED(this->fence->SetEventOnCompletion(this->fenceValues[frameIndex], this->fenceEvent)))
			{
				// Should Exit or Throw an error
				Logger::throw_exception("Waiting for fence value failed.");
				return;
			}
			WaitForSingleObjectEx(this->fenceEvent, INFINITE, FALSE);
		}
	}
}
