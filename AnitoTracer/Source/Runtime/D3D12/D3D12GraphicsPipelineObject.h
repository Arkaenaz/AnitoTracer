#pragma once

#include "D3D12PipelineStateObject.h"

namespace Anito
{
	class D3D12GraphicsPipelineObject : public D3D12PipelineStateObject
	{
	public:
		D3D12GraphicsPipelineObject(const D3D12Device& device);

		void create();
		void setRootSignature(ID3D12RootSignature* rootSignature);
		void setVertexShader(D3D12_SHADER_BYTECODE shader);
		void setPixelShader(D3D12_SHADER_BYTECODE shader);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC getDesc() const;

	private:
		

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsDesc = {};
	};
}

