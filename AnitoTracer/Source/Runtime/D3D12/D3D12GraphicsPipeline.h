#pragma once

namespace Anito
{
	class D3D12GraphicsPipeline
	{
	public:
		D3D12GraphicsPipeline();

		void setRootSignature(ID3D12RootSignature* rootSignature);
		void setVertexShader(D3D12_SHADER_BYTECODE shader);
		void setPixelShader(D3D12_SHADER_BYTECODE shader);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC getDesc() const;

	private:
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsDesc = {};
	};
}

