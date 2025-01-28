#include "pch.h"
#include "Primitive.h"
#include "CommandContext.h"

#include <array>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <filesystem>

using namespace Graphics;
using namespace Math;

Primitive::Primitive(std::string name, ObjectType type, RootSignature* rs)
{
    this->rootSignature = rs;
	this->init(type);
}

Primitive::~Primitive()
{
    // Free up resources in an orderly fashion
    vertexBuffer.Destroy();
    indexBuffer.Destroy();
}

void Primitive::init(ObjectType type)
{
	switch (type)
	{
		case CUBE:
			this->createCube();
			break;
		default:
			break;
	}
}

void Primitive::createCube()
{
    std::array<Vertex, 8> vertices =
    {
        Vertex({ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(Colors::White) }),
        Vertex({ XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT4(Colors::Black) }),
        Vertex({ XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT4(Colors::Red) }),
        Vertex({ XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT4(Colors::Blue) }),
        Vertex({ XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT4(Colors::Yellow) }),
        Vertex({ XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT4(Colors::Cyan) }),
        Vertex({ XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT4(Colors::Magenta) })
    };

    std::array<std::uint16_t, 36> indices =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };

    vertexBuffer.Create(L"vertex buff", 8, sizeof(Vertex), vertices.data());
    indexBuffer.Create(L"index buff", 36, sizeof(std::uint16_t), indices.data());

    D3D12_INPUT_ELEMENT_DESC mInputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    DXGI_FORMAT ColorFormat = g_SceneColorBuffer.GetFormat();
    DXGI_FORMAT DepthFormat = g_SceneDepthBuffer.GetFormat();

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    std::wstring fullPath = std::filesystem::absolute(L"Source/shaders.hlsl");

    D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
    D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

    graphicsPSO.SetRootSignature(*rootSignature);
    graphicsPSO.SetRasterizerState(RasterizerDefault);
    graphicsPSO.SetBlendState(BlendDisable);
    graphicsPSO.SetDepthStencilState(DepthStateDisabled);
    graphicsPSO.SetInputLayout(_countof(mInputLayout), mInputLayout);
    graphicsPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    graphicsPSO.SetRenderTargetFormat(ColorFormat, DXGI_FORMAT_UNKNOWN);
    graphicsPSO.SetVertexShader(CD3DX12_SHADER_BYTECODE(vertexShader));
    graphicsPSO.SetPixelShader(CD3DX12_SHADER_BYTECODE(pixelShader));
    graphicsPSO.Finalize();
}

void Primitive::update(float deltaTime, RECT viewport)
{
}

void Primitive::draw(GraphicsContext& context, Matrix4 viewMat)
{
    context.SetPipelineState(graphicsPSO);
    context.SetRootSignature(*rootSignature);

    context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context.SetVertexBuffer(0, vertexBuffer.VertexBufferView());
    context.SetIndexBuffer(indexBuffer.IndexBufferView());
    context.SetDynamicConstantBufferView(0, sizeof(viewMat), &viewMat);

    context.DrawIndexedInstanced(36, 1, 0, 0, 0);

    context.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_PRESENT);
}
