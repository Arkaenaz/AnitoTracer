#include "RenderSystem.h"

#include "Logger.h"
namespace Anito
{
	SwapChain* RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height)
	{
		return new SwapChain(this, hwnd, width, height);
	}

	DeviceContext* RenderSystem::getImmediateDeviceContext()
	{
		return this->immediateDeviceContext;
	}

	IDXGIFactory7* RenderSystem::getDXFactory()
	{
		return this->dxgiFactory;
	}

	ID3D12Device14* RenderSystem::getDXDevice()
	{
		return this->d3d12Device;
	}

	RenderSystem::RenderSystem()
	{
		HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&this->dxgiFactory));
		if (!Logger::debug(this, hr))
		{
			Logger::error(this, "DXGIFactory not created successfully");
		}

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

		hr = D3D12CreateDevice(this->dxgiAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device10), (void**)&this->d3d12Device);

		if (!Logger::debug(this, hr))
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
		hr = this->d3d12Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
			&featLevels, sizeof(featLevels));
		if (SUCCEEDED(hr))
		{
			this->featureLevel = featLevels.MaxSupportedFeatureLevel;
		}

		this->immediateDeviceContext = new DeviceContext(this, this->d3d12Device);

		//this->d3d12Device->QueryInterface(IID_PPV_ARGS(&this->dxgiDevice));

		Logger::debug(this, "Initialized");
	}

	RenderSystem::~RenderSystem()
	{
		this->d3d12Device->Release();

		//this->dxgiDevice->Release();
		this->dxgiAdapter->Release();
		this->dxgiFactory->Release();
	};
}

