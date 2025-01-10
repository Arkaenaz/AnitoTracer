#include "AnitoTracerPCH.h"
#include "D3D12RenderSystem.h"

#include "D3D12Device.h"

namespace Anito
{
	D3D12SwapChain* D3D12RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height)
	{
		return new D3D12SwapChain(*this->device, this, width, height, hwnd);
	}

	// TODO : Create Custom Rasterizer States and Blend States
	D3D12PipelineState* D3D12RenderSystem::createPipelineState(const D3D12GraphicsPipeline& graphicsPipeline)
	{
		return new D3D12PipelineState(*this->device, graphicsPipeline);
	}

	D3D12VertexBuffer* D3D12RenderSystem::createVertexBuffer(void* listVertices, UINT sizeVertex, UINT sizeList)
	{
		D3D12VertexBuffer* vertexBuffer = nullptr;
		vertexBuffer = new D3D12VertexBuffer();
		vertexBuffer->load(*this->device, listVertices, sizeVertex, sizeList);

		return vertexBuffer;
	}

	D3D12Device* D3D12RenderSystem::getDevice()
	{
		return this->device;
	}

	D3D12CommandContext* D3D12RenderSystem::getDXContext()
	{
		return this->commandContext;
	}

	IDXGIFactory7* D3D12RenderSystem::getDXFactory()
	{
		return this->dxgiFactory;
	}

	D3D12RenderSystem* D3D12RenderSystem::getInstance()
	{
		return P_SHARED_INSTANCE;
	}

	bool D3D12RenderSystem::initialize(bool useWarpDevice)
	{
		if (P_SHARED_INSTANCE)
		{
			Logger::error("D3D12 Render System already created");
			return false;
		}
		P_SHARED_INSTANCE = new D3D12RenderSystem(useWarpDevice);
	}

	void D3D12RenderSystem::destroy()
	{
		delete P_SHARED_INSTANCE;
	}

	D3D12RenderSystem* D3D12RenderSystem::P_SHARED_INSTANCE = nullptr;
	D3D12RenderSystem::D3D12RenderSystem(bool useWarpDevice)
	{
		// Create a factory
		UINT createFactoryFlags = 0;

#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&this->dxgiFactory));
		Logger::logHResult(this, hr);
		if (FAILED(hr))
		{
			Logger::error(this, "DXGIFactory not created successfully");
		}

		// Query for an adapter
		if (useWarpDevice)
		{
			hr = this->dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter));
			// Check to see if the adapter supports Direct3D 12,
			// but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(this->dxgiAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device10), nullptr)))
			{
				Logger::debug(this, "Device is supported");
			}
		}
		else {
			for (UINT adapterIndex = 0; SUCCEEDED(this->dxgiFactory->EnumAdapterByGpuPreference(
				adapterIndex,
				DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&this->dxgiAdapter)));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC3 desc;
				hr = this->dxgiAdapter->GetDesc3(&desc);

				if (FAILED(hr))
					continue;

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					continue;
				}

				// Check to see if the adapter supports Direct3D 12,
				// but don't create the actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(this->dxgiAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device10), nullptr)))
				{
					Logger::debug(this, "Device is supported");
					break;
				}
			}
		}

		this->device = new D3D12Device(this->dxgiAdapter);
		this->commandContext = new D3D12CommandContext(*this->device);

		//this->device->QueryInterface(IID_PPV_ARGS(&this->dxgiDevice));

		if (this->commandContext == nullptr)
		{
			Logger::debug(this, "Device Context is null");
		}
		Logger::debug(this, "Initialized");
	}

	D3D12RenderSystem::~D3D12RenderSystem()
	{
		delete this->commandContext;
		delete this->device;

		this->dxgiAdapter->Release();
		this->dxgiFactory->Release();

		Logger::debug(this, "Destroyed");
	};
}

