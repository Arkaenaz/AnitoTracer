#include "pch.h"
#include "GameCore.h"
#include "GraphicsCore.h"
#include "SystemTime.h"
#include "TextRenderer.h"
#include "GameInput.h"
#include "CommandContext.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "BufferManager.h"
#include "Camera.h"

#include <array>
#include <DirectXColors.h>
#include "GameInput.h"
#include <filesystem>
#include <d3dcompiler.h>

#include "CameraController.h"

using namespace GameCore;
using namespace Graphics;
using namespace Math;

class AnitoTracer : public GameCore::IGameApp
{
public:

    AnitoTracer()
    {
    }

    virtual void Startup(void) override;
    virtual void Cleanup(void) override;

    virtual void Update(float deltaT) override;
    virtual void RenderScene(void) override;

private:
    RootSignature rootSignature;
    StructuredBuffer vertexBuffer;
    ByteAddressBuffer indexBuffer;
    GraphicsPSO graphicsPSO;

    Camera camera;
    FlyingFPSCamera* cameraController;
    Matrix4 viewProjMatrix;
    D3D12_VIEWPORT mainViewport;
    D3D12_RECT mainScissor;

    float radius = 5.0f;

    float xRotate = 0.0f;
    float xLast = 0.0f;
    float xDiff = 0.0f;

    float yRotate = 0.0f;
    float yLast = 0.0f;
    float yDiff = 0.0f;
};

CREATE_APPLICATION(AnitoTracer)

void AnitoTracer::Startup(void)
{
    // Setup your data
    rootSignature.Reset(1, 0);
    rootSignature[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
    rootSignature.Finalize(L"box signature", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    struct Vertex
    {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
    };

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

    ID3D10Blob* vertexShader;
    ID3D10Blob* pixelShader;

    std::wstring fullPath = std::filesystem::absolute(L"Source/shaders.hlsl");

    D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
    D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

    graphicsPSO.SetRootSignature(rootSignature);
    graphicsPSO.SetRasterizerState(RasterizerDefault);
    graphicsPSO.SetBlendState(BlendDisable);
    graphicsPSO.SetDepthStencilState(DepthStateDisabled);
    graphicsPSO.SetInputLayout(_countof(mInputLayout), mInputLayout);
    graphicsPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    graphicsPSO.SetRenderTargetFormat(ColorFormat, DXGI_FORMAT_UNKNOWN);
    graphicsPSO.SetVertexShader(CD3DX12_SHADER_BYTECODE(vertexShader));
    graphicsPSO.SetPixelShader(CD3DX12_SHADER_BYTECODE(pixelShader));
    graphicsPSO.Finalize();

    camera.SetZRange(1.0f, 10000.0f);
    cameraController = new FlyingFPSCamera(camera, Vector3(kYUnitVector));
}

void AnitoTracer::Cleanup(void)
{
    // Free up resources in an orderly fashion
    vertexBuffer.Destroy();
    indexBuffer.Destroy();
}

void AnitoTracer::Update(float deltaT)
{
    ScopedTimer _prof(L"Update State");

    // Update something
    if (GameInput::IsPressed(GameInput::kMouse0) || GameInput::IsPressed(GameInput::kMouse1)) {
        // Make each pixel correspond to a quarter of a degree.
        float dx = GameInput::GetAnalogInput(GameInput::kAnalogMouseX) - xLast;
        float dy = GameInput::GetAnalogInput(GameInput::kAnalogMouseY) - yLast;

        if (GameInput::IsPressed(GameInput::kMouse0))
        {
            // Update angles based on input to orbit camera around box.
            xRotate += (dx - xDiff);
            yRotate += (dy - yDiff);
            yRotate = (std::max)(-XM_PIDIV2 + 0.1f, yRotate);
            yRotate = (std::min)(XM_PIDIV2 - 0.1f, yRotate);
        }
        else
        {
            radius += dx - dy - (xDiff - yDiff);
        }

        xDiff = dx;
        yDiff = dy;

        xLast += GameInput::GetAnalogInput(GameInput::kAnalogMouseX);
        yLast += GameInput::GetAnalogInput(GameInput::kAnalogMouseY);
    }
    else
    {
        xDiff = 0.0f;
        yDiff = 0.0f;
        xLast = 0.0f;
        yLast = 0.0f;
    }


    float x = radius * cosf(yRotate) * sinf(xRotate);
    float y = radius * sinf(yRotate);
    float z = radius * cosf(yRotate) * cosf(xRotate);

    cameraController->Update(deltaT);

    camera.SetEyeAtUp({ x, y, z }, Vector3(kZero), Vector3(kYUnitVector));
    camera.Update();

    viewProjMatrix = camera.GetViewProjMatrix();

    mainViewport.Width = (float)g_SceneColorBuffer.GetWidth();
    mainViewport.Height = (float)g_SceneColorBuffer.GetHeight();
    mainViewport.MinDepth = 0.0f;
    mainViewport.MaxDepth = 1.0f;

    mainScissor.left = 0;
    mainScissor.top = 0;
    mainScissor.right = (LONG)g_SceneColorBuffer.GetWidth();
    mainScissor.bottom = (LONG)g_SceneColorBuffer.GetHeight();
}

void AnitoTracer::RenderScene(void)
{
    GraphicsContext& gfxContext = GraphicsContext::Begin(L"Scene Render");
    
    gfxContext.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);

    gfxContext.SetViewportAndScissor(mainViewport, mainScissor);

    gfxContext.ClearColor(g_SceneColorBuffer);

    /*gfxContext.TransitionResource(g_SceneDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
    gfxContext.ClearDepth(g_SceneDepthBuffer);

    gfxContext.TransitionResource(g_SceneDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_READ, true);*/
    gfxContext.SetRenderTarget(g_SceneColorBuffer.GetRTV()/*, g_SceneDepthBuffer.GetDSV_DepthReadOnly()*/);

    gfxContext.SetPipelineState(graphicsPSO);

    gfxContext.SetRootSignature(rootSignature);

    gfxContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    gfxContext.SetVertexBuffer(0, vertexBuffer.VertexBufferView());

    gfxContext.SetIndexBuffer(indexBuffer.IndexBufferView());

    gfxContext.SetDynamicConstantBufferView(0, sizeof(viewProjMatrix), &viewProjMatrix);

    gfxContext.DrawIndexedInstanced(36, 1, 0, 0, 0);

    gfxContext.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_PRESENT);

    gfxContext.Finish();
}
