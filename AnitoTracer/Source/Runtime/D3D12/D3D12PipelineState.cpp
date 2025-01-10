#include "AnitoTracerPCH.h"
#include "D3D12PipelineState.h"

#include <d3dcompiler.h>
#include <filesystem>

#include "D3D12RenderSystem.h"

namespace Anito
{
	D3D12PipelineState::D3D12PipelineState(D3D12RenderSystem* system) : system(system)
	{
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &this->signature, &this->error);
        this->system->getDevice()->get()->CreateRootSignature(0, this->signature->GetBufferPointer(), this->signature->GetBufferSize(), IID_PPV_ARGS(&this->rootSignature));

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif


        std::wstring fullPath = std::filesystem::absolute(L"Source/shaders.hlsl");
        Logger::log(fullPath);
        D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &this->vertexShader, nullptr);
        D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &this->pixelShader, nullptr);

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = this->rootSignature;
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(this->vertexShader);
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(this->pixelShader);
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        HRESULT hr = this->system->getDevice()->get()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&this->pipelineState));
        if (SUCCEEDED(hr))
        {
            Logger::debug(this, "Pipeline State created successfully");
        }
        else
        {
            Logger::error(this, "Pipeline State not created successfully");
        }
	}

	D3D12PipelineState::~D3D12PipelineState()
	{
        this->pipelineState->Release();
        this->pixelShader->Release();
        this->vertexShader->Release();
        this->rootSignature->Release();
        if (this->signature)
			this->signature->Release();
        if (this->error)
			this->error->Release();
	}

	ID3D12RootSignature* D3D12PipelineState::getRootSignature()
	{
        return this->rootSignature;
	}

	ID3D12PipelineState* D3D12PipelineState::getDXState()
	{
        return this->pipelineState;
	}
}
