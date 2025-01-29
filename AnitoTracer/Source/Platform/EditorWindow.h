#pragma once

#include "Window.h"
#include "Runtime/D3D12/D3D12GraphicsPipelineObject.h"

#include "Runtime/D3D12/D3D12SwapChain.h"
#include "Runtime/D3D12/D3D12VertexBuffer.h"

namespace Anito
{

	struct Viewport
	{
		float left;
		float top;
		float right;
		float bottom;
	};

	struct RayGenConstantBuffer
	{
		Viewport viewport;
		Viewport stencil;
	};

	class EditorWindow : public Window
	{
	public:
		virtual void onCreate() override;
		virtual void onUpdate() override;
		virtual void onRender() override;
		virtual void onResize(UINT width, UINT height) override;
		virtual void onDestroy() override;
		virtual void onFocus() override;
		virtual void onKillFocus() override;

		void initializeEngine();
		void CreateRootSignatures();
		void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
		void CreateRaytracingPipelineStateObject();
		void createAccelerationStructures(ID3D12GraphicsCommandList10* cmdList);

	private:
		D3D12SwapChain* swapChain;
		D3D12VertexBuffer* vertexBuffer;

		D3D12GraphicsPipelineObject* graphicsPipeline;
		ID3D12RootSignature* rootSignature;

		D3D12Buffer* topLevelAS;
		D3D12Buffer* bottomLevelAS;

		// TEMP
		ID3D12StateObject* m_dxrStateObject;

		ID3D12RootSignature* m_raytracingGlobalRootSignature;
		ID3D12RootSignature* m_raytracingLocalRootSignature;



		RayGenConstantBuffer m_rayGenCB;

		static const wchar_t* c_hitGroupName;
		static const wchar_t* c_raygenShaderName;
		static const wchar_t* c_closestHitShaderName;
		static const wchar_t* c_missShaderName;
		ID3D12Resource* m_missShaderTable;
		ID3D12Resource* m_hitGroupShaderTable;
		ID3D12Resource* m_rayGenShaderTable;
		// TEMP


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
