#pragma once

#include "Window.h"
#include "Runtime/D3D12/D3D12PipelineState.h"

#include "Runtime/D3D12/D3D12SwapChain.h"
#include "Runtime/D3D12/D3D12VertexBuffer.h"

namespace Anito
{
	class EditorWindow : public Window
	{
	public:
		virtual void onCreate() override;
		virtual void onUpdate() override;
		virtual void onRender() override;
		virtual void onDestroy() override;
		virtual void onFocus() override;
		virtual void onKillFocus() override;

		void initializeEngine();
		void beginFrame(ID3D12GraphicsCommandList7* cmdList);
		void endFrame(ID3D12GraphicsCommandList7* cmdList);

	private:
		D3D12SwapChain* swapChain;
		D3D12VertexBuffer* vertexBuffer;
		D3D12PipelineState* pipelineState;

		struct vec3
		{
			float x, y, z;
		};

		struct vec4
		{
			float x, y, z, w;
		};

		struct Vertex
		{
			vec3 position;
			vec4 color;
		};
	};
}
