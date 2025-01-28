#include "pch.h"
#include "GameCore.h"
#include "GraphicsCore.h"
#include "GameInput.h"
#include "CommandContext.h"
#include "RootSignature.h"
#include "Camera.h"

#include "Primitives/Primitive.h"

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

    Primitive* cube;

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

    cube = new Primitive("primitive cube", Primitive::CUBE, &rootSignature);

    camera.SetZRange(1.0f, 10000.0f);
    cameraController = new FlyingFPSCamera(camera, Vector3(kYUnitVector));
}

void AnitoTracer::Cleanup(void)
{
    delete cube;
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

    cube->draw(gfxContext, viewProjMatrix);

    gfxContext.Finish();
}
