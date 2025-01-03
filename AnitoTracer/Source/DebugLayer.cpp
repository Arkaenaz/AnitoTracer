#include "DebugLayer.h"

#include "Logger.h"

namespace Anito
{
	DebugLayer* DebugLayer::getInstance()
	{
		return P_SHARED_INSTANCE;
	}
	void DebugLayer::initialize()
	{
		if (P_SHARED_INSTANCE)
		{
			Logger::debug("Debug Layer already created");
		}
		P_SHARED_INSTANCE = new DebugLayer();
	}
	void DebugLayer::destroy()
	{
		delete P_SHARED_INSTANCE;
	}

	DebugLayer* DebugLayer::P_SHARED_INSTANCE = nullptr;
	DebugLayer::DebugLayer()
	{
#ifdef _DEBUG
		// Initialize D3D12 Debug Layer
		if (Logger::debug(this, D3D12GetDebugInterface(IID_PPV_ARGS(&this->d3d12Debug))))
		{
			this->d3d12Debug->EnableDebugLayer();
			Logger::debug(this, "Enabled D3D12 Debug Layer");
		}
		// Initialize DXGI Debug Layer
		if (Logger::debug(this, DXGIGetDebugInterface1(0, IID_PPV_ARGS(&this->dxgiDebug))))
		{
			this->dxgiDebug->EnableLeakTrackingForThread();
			Logger::debug(this, "Enabled DXGI Leak Tracking for Thread");
		}
#endif
		Logger::debug(this, "Initialized");
	}
	DebugLayer::~DebugLayer()
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
		this->d3d12Debug->Release();
#endif
		P_SHARED_INSTANCE = nullptr;
		Logger::debug(this, "Destroyed");
	}
	DebugLayer::DebugLayer(const DebugLayer&) {}
}
