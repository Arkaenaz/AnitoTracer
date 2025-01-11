#include "AnitoTracerPCH.h"
#include "D3D12DebugLayer.h"

namespace Anito
{
	D3D12DebugLayer* D3D12DebugLayer::getInstance()
	{
		return P_SHARED_INSTANCE;
	}
	void D3D12DebugLayer::initialize()
	{
		if (P_SHARED_INSTANCE)
		{
			Logger::debug("Debug Layer already created");
		}
		P_SHARED_INSTANCE = new D3D12DebugLayer();
	}
	void D3D12DebugLayer::destroy()
	{
		delete P_SHARED_INSTANCE;
	}

	D3D12DebugLayer* D3D12DebugLayer::P_SHARED_INSTANCE = nullptr;
	D3D12DebugLayer::D3D12DebugLayer()
	{
#ifdef _DEBUG
		// Initialize D3D12 Debug Layer
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&this->debugController))))
		{
			this->debugController->EnableDebugLayer();
			Logger::debug(this, "Enabled D3D12 Debug Layer");
		}
		// Initialize DXGI Debug Layer
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&this->dxgiDebug))))
		{
			this->dxgiDebug->EnableLeakTrackingForThread();
			Logger::debug(this, "Enabled DXGI Leak Tracking for Thread");
		}

		IDXGIInfoQueue* dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
		{
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
		dxgiInfoQueue->Release();
#endif
		Logger::debug(this, "Initialized");
	}
	D3D12DebugLayer::~D3D12DebugLayer()
	{
#ifdef _DEBUG
		if (this->dxgiDebug)
		{
			OutputDebugStringW(L"DXGI Reports living device objects:\n");
			this->dxgiDebug->ReportLiveObjects(
				DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
			);
		}
		this->dxgiDebug->Release();
		this->debugController->Release();
#endif
		P_SHARED_INSTANCE = nullptr;
		Logger::debug(this, "Destroyed");
	}
}
