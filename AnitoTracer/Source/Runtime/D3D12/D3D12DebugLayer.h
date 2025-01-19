#pragma once

namespace Anito
{
	class D3D12DebugLayer
	{
	private:
#ifdef _DEBUG
/*#if defined(NTDDI_WIN11_ZN)
		ID3D12Debug6* debugController;
#else*/
		ID3D12Debug5* debugController;
//#endif
		IDXGIDebug1* dxgiDebug;
#endif

	public:
		static D3D12DebugLayer* getInstance();
		static void initialize();
		static void destroy();

	private:
		static D3D12DebugLayer* P_SHARED_INSTANCE;

		D3D12DebugLayer();
		~D3D12DebugLayer();
		D3D12DebugLayer(D3D12DebugLayer const&) {}
		D3D12DebugLayer& operator = (D3D12DebugLayer const&) {}
	};
}