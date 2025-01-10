#pragma once

#include "D3D12Device.h"
#include "D3D12CommandContext.h"
#include "D3D12GraphicsPipeline.h"
#include "D3D12SwapChain.h"
#include "D3D12VertexBuffer.h"
#include "D3D12PipelineState.h"

namespace Anito
{
	class D3D12PipelineState;
	class D3D12CommandContext;
	class D3D12SwapChain;
	class D3D12VertexBuffer;
	class D3D12RenderSystem
	{
	public:
		D3D12SwapChain* createSwapChain(HWND hwnd, UINT width, UINT height);
		D3D12PipelineState* createPipelineState(const D3D12GraphicsPipeline& graphicsPipeline);
		D3D12VertexBuffer* createVertexBuffer(void* listVertices, UINT sizeVertex, UINT sizeList);
		D3D12Device* getDevice();
		D3D12CommandContext* getDXContext();
		IDXGIFactory7* getDXFactory();

	private:
		friend class D3D12SwapChain;

		D3D12Device* device;
		D3D12CommandContext* commandContext;

		IDXGIAdapter4* dxgiAdapter;
		IDXGIFactory7* dxgiFactory;

	public:
		static D3D12RenderSystem* getInstance();
		static bool initialize(bool useWarpDevice = false);
		static void destroy();

	private:
		static D3D12RenderSystem* P_SHARED_INSTANCE;

		D3D12RenderSystem(bool useWarpDevice = false);
		~D3D12RenderSystem();
		D3D12RenderSystem(D3D12RenderSystem const&) {}
		D3D12RenderSystem& operator = (D3D12RenderSystem const&) {}
	};
}