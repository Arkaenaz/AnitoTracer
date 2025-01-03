#pragma once

#include "WinInclude.h"

namespace Anito
{
	class DebugLayer
	{
	public:
		
	private:
#ifdef _DEBUG
		ID3D12Debug6* d3d12Debug;
		IDXGIDebug1* dxgiDebug;
#endif

	public:
		static DebugLayer* getInstance();
		static void initialize();
		static void destroy();

	private:
		static DebugLayer* P_SHARED_INSTANCE;

		DebugLayer();
		~DebugLayer();
		DebugLayer(DebugLayer const&);
		DebugLayer& operator = (DebugLayer const&);
	};
}