#include "SwapChain.h"

#include "RenderSystem.h"
#include "Logger.h"

namespace Anito
{
	SwapChain::SwapChain(RenderSystem* system, HWND hwnd, UINT width, UINT height) : system(system)
	{
		ID3D12Device14* device = this->system->getDXDevice();
		DeviceContext* deviceContext = this->system->getImmediateDeviceContext();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		// Create the swap chain for the window indicated by HWND parameter
		IDXGISwapChain1* swapChain;
		HRESULT hr = this->system->dxgiFactory->CreateSwapChainForHwnd(deviceContext->getCommandQueue(), 
			hwnd, &swapChainDesc, nullptr, nullptr, &swapChain);

		if (Logger::debug(this, hr))
		{
			Logger::debug(this, "SwapChain created successfully");
		}
		else
		{
			Logger::error(this, "SwapChain not created successfully");
		}

		this->swapChain = static_cast<IDXGISwapChain4*>(swapChain);

		{
			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = 2;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			if (Logger::debug(this, device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&this->renderTargetViewHeap))))
			{
				Logger::debug(this, "Render Target View created successfully");
			}
			else
			{
				Logger::error(this, "Render Target View not created successfully");
			}

			this->rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}
	}

	SwapChain::~SwapChain()
	{
		this->swapChain->Release();
	}

	void SwapChain::cleanRenderTarget()
	{
		if (this->renderTargetViewHeap)
		{
			this->renderTargetViewHeap->Release();
			this->renderTargetViewHeap = nullptr;
		}
	}

	void SwapChain::resizeBuffers(UINT bufferCount, UINT width, UINT height)
	{
		//m_renderTexture->resizeResources(width, height);
		//m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	}

	void SwapChain::createRenderTarget()
	{
		/*ID3D11Texture2D* buffer = NULL;
		HRESULT result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

		if (!Logger::log(this, result))
			Logger::throw_exception("Swap Chain Fail");

		result = this->m_system->m_D3DDevice->CreateRenderTargetView(buffer, NULL, &m_renderTexture->m_renderTargetView);
		if (!Logger::log(this, result))
			Logger::throw_exception("Render Target View not created successfully.");

		buffer->Release();*/
	}

	bool SwapChain::present(bool vsync)
	{
		this->swapChain->Present(vsync, NULL);
		return true;
	}
}