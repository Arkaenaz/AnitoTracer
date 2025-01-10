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
			ARRAYSIZE(featureLevels), featureLevels, D3D_FEATURE_LEVEL_11_0
		};

		this->featureLevel = D3D_FEATURE_LEVEL_11_0;

		hr = device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
			&featLevels, sizeof(featLevels));

		if (SUCCEEDED(hr))
		{
			this->featureLevel = featLevels.MaxSupportedFeatureLevel;
		}

		ID3D12InfoQueue* infoQueue;
		if (SUCCEEDED(this->device->QueryInterface(&infoQueue)))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		}
	}

	D3D12Device::~D3D12Device()
	{
		this->device->Release();
	}

	ID3D12Device10* D3D12Device::get() const
	{
		return this->device;
	}
}
