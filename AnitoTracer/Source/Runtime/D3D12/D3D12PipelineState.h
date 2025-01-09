#pragma once

namespace Anito {
	class D3D12RenderSystem;
	class D3D12PipelineState
	{
	public:
		D3D12PipelineState(D3D12RenderSystem* system);
		~D3D12PipelineState();
		D3D12PipelineState(const D3D12PipelineState&) = delete;
		D3D12PipelineState& operator=(const D3D12PipelineState&) = delete;

		ID3D12RootSignature* getRootSignature();
		ID3D12PipelineState* getDXState();
	private:
		D3D12RenderSystem* system;
		ID3D12PipelineState* pipelineState;
		ID3D12RootSignature* rootSignature;

		ID3D10Blob* signature;
		ID3D10Blob* error;
		ID3D10Blob* vertexShader;
		ID3D10Blob* pixelShader;
	};
}

