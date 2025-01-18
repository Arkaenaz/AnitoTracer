#include "AnitoTracerPCH.h"
#include "D3D12SwapChain.h"

#include "D3D12RenderSystem.h"

namespace Anito
{
	D3D12SwapChain::D3D12SwapChain(const D3D12Device& device, D3D12RenderSystem* system, UINT width, UINT height, HWND handle) :
		device(device), frameIndex(0), rtvDescriptorSize(0)
	{
		D3D12CommandContext* deviceContext = system->getDXContext();

		// Check if Tearing is supported
		BOOL allowTearing = true;
		this->swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		if (FAILED(system->getDXFactory()->CheckFeatureSupport(
			DXGI_FEATURE_PRESENT_ALLOW_TEARING,
			&allowTearing, sizeof(allowTearing))))
		{
			Logger::debug(this, "Tearing is not supported");
			allowTearing = false;
		}
		else
		{
			Logger::debug(this, "Tearing is supported");
		}

		this->swapChainFlags |= allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		// Create the swap chain for the window indicated by HWND parameter
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = FrameCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = this->swapChainFlags;

		IDXGISwapChain1* swapChain;
		HRESULT hr = system->getDXFactory()->CreateSwapChainForHwnd(deviceContext->getCommandQueue()->get(),
			handle, &swapChainDesc, nullptr, nullptr, &swapChain);

		if (SUCCEEDED(hr))
		{
			Logger::debug(this, "SwapChain created successfully");
		}
		else
		{
			Logger::error(this, "SwapChain not created successfully");
		}

		system->getDXFactory()->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER);
		swapChain->QueryInterface(&this->swapChain);
		swapChain->Release();

		this->frameIndex = this->swapChain->GetCurrentBackBufferIndex();
		{
			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = FrameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			if (SUCCEEDED(device.get()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&this->renderTargetViewHeap))))
			{
				Logger::debug(this, "Render Target View created successfully");
			}
			else
			{
				Logger::error(this, "Render Target View not created successfully");
			}

			this->rtvDescriptorSize = device.get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(this->renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a RTV for each frame.
			for (size_t i = 0; i < FrameCount; i++)
			{
				ID3D12Resource2* renderTarget;
				this->swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&renderTarget));
				this->renderTargets[i] = new D3D12Resource(renderTarget);
				device.get()->CreateRenderTargetView(this->renderTargets[i]->get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, this->rtvDescriptorSize);
				std::wstring name = L"Anito Render Target " + std::to_wstring(i);
				this->renderTargets[i]->get()->SetName(name.c_str());
			}
		}
	}

	D3D12SwapChain::~D3D12SwapChain()
	{
		for (size_t i = 0; i < ARRAYSIZE(this->renderTargets); i++)
		{
			delete this->renderTargets[i];
		}
		this->renderTargetViewHeap->Release();
		this->swapChain->Release();
	}

	void D3D12SwapChain::cleanRenderTarget()
	{
		if (this->renderTargetViewHeap)
		{
			this->renderTargetViewHeap->Release();
		}
		for (size_t i = 0; i < FrameCount; i++)
		{
			delete this->renderTargets[i];
		}
	}

	void D3D12SwapChain::resizeBuffers(UINT width, UINT height)
	{
		this->swapChain->ResizeBuffers(FrameCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, this->swapChainFlags);
	}

	void D3D12SwapChain::createRenderTarget()
	{
		{
			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = FrameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			if (SUCCEEDED(device.get()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&this->renderTargetViewHeap))))
			{
				this->rtvDescriptorSize = device.get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			}
		}

		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(this->renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a RTV for each frame.
			for (size_t i = 0; i < FrameCount; i++)
			{
				ID3D12Resource2* renderTarget;
				this->swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&renderTarget));
				this->renderTargets[i] = new D3D12Resource(renderTarget);

				device.get()->CreateRenderTargetView(this->renderTargets[i]->get(), nullptr, rtvHandle);

				std::wstring name = L"Anito Render Target " + std::to_wstring(i);
				this->renderTargets[i]->get()->SetName(name.c_str());

				rtvHandle.Offset(1, this->rtvDescriptorSize);
			}
		}
	}

	bool D3D12SwapChain::present(bool vsync)
	{
		this->swapChain->Present(vsync, NULL);
		return true;
	}

	void D3D12SwapChain::updateFrameIndex()
	{
		this->frameIndex = this->swapChain->GetCurrentBackBufferIndex();
	}

	UINT D3D12SwapChain::getFrameIndex()
	{
		return this->frameIndex;
	}

	UINT D3D12SwapChain::getRTVDescriptorSize()
	{
		return this->rtvDescriptorSize;
	}

	D3D12Resource* D3D12SwapChain::getRenderTarget(UINT frameIndex)
	{
		return this->renderTargets[frameIndex];
	}

	D3D12Resource* D3D12SwapChain::getCurrentRenderTarget()
	{
		return this->renderTargets[this->frameIndex];
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12SwapChain::getRenderTargetViewHandle()
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE({ this->renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(this->frameIndex), this->rtvDescriptorSize });
	}
}
