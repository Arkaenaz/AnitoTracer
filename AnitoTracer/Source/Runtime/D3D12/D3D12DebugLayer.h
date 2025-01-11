#pragma once

namespace Anito
{
	class D3D12DebugLayer
	{
	public:
		
	private:
#ifdef _DEBUG
		ID3D12Debug6* debugController;
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