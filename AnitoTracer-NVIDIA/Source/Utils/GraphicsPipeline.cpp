#include "GraphicsPipeline.h"

D3D12_ROOT_SIGNATURE_DESC DirectXUtil::GraphicsPipeline::createGraphicsRootSigDesc()
{
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 0;
	desc.pParameters = nullptr;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	return desc;
}

D3D12_RASTERIZER_DESC DirectXUtil::GraphicsPipeline::createDefaultRasterizerDesc()
{
	D3D12_RASTERIZER_DESC desc = {};
    desc.FillMode = D3D12_FILL_MODE_SOLID;
    desc.CullMode = D3D12_CULL_MODE_BACK;
    desc.FrontCounterClockwise = FALSE;
    desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    desc.DepthClipEnable = TRUE;
    desc.MultisampleEnable = FALSE;
    desc.AntialiasedLineEnable = FALSE;
    desc.ForcedSampleCount = 0;
    desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return desc;
}

D3D12_BLEND_DESC DirectXUtil::GraphicsPipeline::createDefaultBlendDesc()
{
    D3D12_BLEND_DESC desc = {};
    desc.AlphaToCoverageEnable = FALSE;
    desc.IndependentBlendEnable = FALSE;
    desc.RenderTarget[0].BlendEnable = FALSE;
    desc.RenderTarget[0].LogicOpEnable = FALSE;
    desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
    desc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
    desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return desc;
}

D3D12_SHADER_BYTECODE DirectXUtil::GraphicsPipeline::compileVertexShader(const wchar_t* fileName,
	const char* entryPointName, ID3DBlob* shaderBlob)
{
#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ID3DBlob* errorBlob;

    if (!SUCCEEDED(D3DCompileFromFile(fileName, nullptr, nullptr, entryPointName, "vs_5_0", compileFlags, 0, &shaderBlob, &errorBlob)))
    {
        if (errorBlob)
            errorBlob->Release();
        return D3D12_SHADER_BYTECODE({ nullptr, 0 });
    }

    return D3D12_SHADER_BYTECODE({ shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() });
}

D3D12_SHADER_BYTECODE DirectXUtil::GraphicsPipeline::compilePixelShader(const wchar_t* fileName,
	const char* entryPointName, ID3DBlob* shaderBlob)
{
#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ID3DBlob* errorBlob;

    if (!SUCCEEDED(D3DCompileFromFile(fileName, nullptr, nullptr, entryPointName, "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob)))
    {
        if (errorBlob)
            errorBlob->Release();
        return D3D12_SHADER_BYTECODE({ nullptr, 0 });
    }

    return D3D12_SHADER_BYTECODE({ shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() });
}
