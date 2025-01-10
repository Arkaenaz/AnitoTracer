#include "AnitoTracerPCH.h"
#include "D3D12RenderSystem.h"

namespace Anito
{
	D3D12SwapChain* D3D12RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height)
	{
		return new D3D12SwapChain(this, hwnd, width, height);
	}

	// TODO : Create Custom Rasterizer States and Blend States
	D3D12PipelineState* D3D12RenderSystem::createPipelineState()
	{
		return new D3D12PipelineState(this);
	}

	D3D12VertexBuffer* D3D12RenderSystem::createVertexBuffer(void* listVertices, UINT sizeVertex, UINT sizeList)
	{
		D3D12VertexBuffer* vertexBuffer = nullptr;
		vertexBuffer = new D3D12VertexBuffer(this);
		vertexBuffer->load(listVertices, sizeVertex, sizeList);

		return vertexBuffer;
	}

	D3D12DeviceContext* D3D12RenderSystem::getDXContext()
	{
		return this->deviceContext;
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
			Logger::debug("D3D12 Render System already created");
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

		ID3D12Device10* device;
		hr = D3D12CreateDevice(this->dxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		Logger::logHResult(this, hr);
		if (FAILED(hr))
		{
			Logger::error(this, "Device not created successfully");
		}

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_12_2,
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
		{
			_countof(featureLevels), featureLevels, D3D_FEATURE_LEVEL_11_0
		};

		this->featureLevel = D3D_FEATURE_LEVEL_11_0;
		
		hr = device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
			&featLevels, sizeof(featLevels));

		if (SUCCEEDED(hr))
		{
			this->featureLevel = featLevels.MaxSupportedFeatureLevel;
		}

		this->deviceContext = new D3D12DeviceContext(device);

		//this->device->QueryInterface(IID_PPV_ARGS(&this->dxgiDevice));

		if (this->deviceContext == nullptr)
		{
			Logger::debug(this, "Device Context is null");
		}
		Logger::debug(this, "Initialized");
	}

	D3D12RenderSystem::~D3D12RenderSystem()
	{
		delete this->deviceContext;

		this->dxgiAdapter->Release();
		this->dxgiFactory->Release();

		Logger::debug(this, "Destroyed");
	};
}

