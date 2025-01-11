#pragma once

#include "Window.h"
#include "Runtime/D3D12/D3D12GraphicsPipeline.h"
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

	private:
		D3D12SwapChain* swapChain;
		D3D12VertexBuffer* vertexBuffer;
		D3D12PipelineState* pipelineState;

		D3D12GraphicsPipeline graphicsPipeline;
		ID3D12RootSignature* rootSignature;
		ID3D10Blob* vertexShader;
		ID3D10Blob* pixelShader;
		ID3D10Blob* signature;
		ID3D10Blob* error;
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
