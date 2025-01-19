#include "AnitoTracerPCH.h"
#include "D3D12Device.h"


namespace Anito
{
	D3D12Device::D3D12Device(IDXGIAdapter4* adapter)
	{
		HRESULT hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&this->device));
		Logger::logHResult(this, hr);
		if (FAILED(hr))
		{
			Logger::error(this, "Device not created successfully");
		}
		else
		{
			Logger::debug(this, "Device created successfully");
		}

#ifdef _DEBUG
		if (SUCCEEDED(this->device->QueryInterface(&infoQueue)))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		}
#endif

		D3D_FEATURE_LEVEL featureLevels[] =
		{
#if defined(NTDDI_WIN10_FE) || defined(USING_D3D12_AGILITY_SDK)
			D3D_FEATURE_LEVEL_12_2,
#endif
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
		{
			ARRAYSIZE(featureLevels), featureLevels, D3D_FEATURE_LEVEL_11_0
		};

		this->featureLevel = D3D_FEATURE_LEVEL_11_0;

		hr = device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
			&featLevels, sizeof(featLevels));

		if (SUCCEEDED(hr))
		{
			this->featureLevel = featLevels.MaxSupportedFeatureLevel;
		}
	}

	D3D12Device::~D3D12Device()
	{
#ifdef _DEBUG
		this->infoQueue->Release();
#endif
		this->device->Release();
	}

	
/*#if defined(NTDDI_WIN11_ZN)
	ID3D12Device10* D3D12Device::get() const
	{
		return this->device;
	}
#else*/
	ID3D12Device9* D3D12Device::get() const
	{
		return this->device;
	}
//#endif
}
