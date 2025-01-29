#include "AnitoTracerPCH.h"
#include "D3D12GraphicsPipelineObject.h"

namespace Anito
{
	D3D12GraphicsPipelineObject::D3D12GraphicsPipelineObject(const D3D12Device& device)
		: D3D12PipelineStateObject(device) {}

	void D3D12GraphicsPipelineObject::create()
	{
		this->graphicsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		this->graphicsDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		this->graphicsDesc.NodeMask = 0;
		this->graphicsDesc.SampleDesc.Count = 1;
		this->graphicsDesc.SampleDesc.Quality = 0;
		this->graphicsDesc.SampleMask = UINT_MAX;

		// TODO : Move everything below to a set function, and create Rasterizer, Blend State and Depth Stencil State Class
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		this->graphicsDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		this->graphicsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		this->graphicsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		this->graphicsDesc.DepthStencilState.DepthEnable = FALSE;
		this->graphicsDesc.DepthStencilState.StencilEnable = FALSE;
		this->graphicsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		this->graphicsDesc.NumRenderTargets = 1;
		this->graphicsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		HRESULT hr = device.get()->CreateGraphicsPipelineState(&this->graphicsDesc, IID_PPV_ARGS(&this->pipelineState));
		if (SUCCEEDED(hr))
		{
			Logger::debug(this, "Graphics Pipeline State created successfully");
		}
		else
		{
			Logger::error(this, "Graphics Pipeline State not created successfully");
		}
	}

	void D3D12GraphicsPipelineObject::setRootSignature(ID3D12RootSignature* rootSignature)
	{
		this->graphicsDesc.pRootSignature = rootSignature;
	}

	void D3D12GraphicsPipelineObject::setVertexShader(D3D12_SHADER_BYTECODE shader)
	{
		this->graphicsDesc.VS = shader;
	}

	void D3D12GraphicsPipelineObject::setPixelShader(D3D12_SHADER_BYTECODE shader)
	{
		this->graphicsDesc.PS = shader;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC D3D12GraphicsPipelineObject::getDesc() const
	{
		return this->graphicsDesc;
	}
}
