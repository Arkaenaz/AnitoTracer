#include "pch.h"
#include "Primitive.h"

#include <array>
#include <d3dcompiler.h>
#include <filesystem>
#include <DirectXColors.h>

#include "Renderer.h"
#include "CommandContext.h"

using namespace DirectX;
using namespace Graphics;
using namespace Math;

Primitive::Primitive(std::string name, ObjectType type)
{
    DXGI_FORMAT ColorFormat = g_SceneColorBuffer.GetFormat();
    DXGI_FORMAT NormalFormat = g_SceneNormalBuffer.GetFormat();
    DXGI_FORMAT DepthFormat = g_SceneDepthBuffer.GetFormat();

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    D3D12_INPUT_ELEMENT_DESC mInputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    std::wstring fullPath = std::filesystem::absolute(L"Shaders/shaders.hlsl");

    D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
    D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

    /**/graphicsPSO.SetRootSignature(Renderer::m_RootSig);
    graphicsPSO.SetRasterizerState(RasterizerDefault);
    graphicsPSO.SetBlendState(BlendDisable);
    graphicsPSO.SetDepthStencilState(DepthStateDisabled);
    graphicsPSO.SetInputLayout(_countof(mInputLayout), mInputLayout);
    graphicsPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    graphicsPSO.SetRenderTargetFormat(ColorFormat, DXGI_FORMAT_UNKNOWN);
    graphicsPSO.SetVertexShader(CD3DX12_SHADER_BYTECODE(vertexShader));
    graphicsPSO.SetPixelShader(CD3DX12_SHADER_BYTECODE(pixelShader));
    graphicsPSO.Finalize();

    this->CreateType(type);
}

Primitive::~Primitive()
{
    // Free up resources in an orderly fashion
    vertexBuffer.Destroy();
    indexBuffer.Destroy();
}

void Primitive::CreateType(ObjectType type)
{
    switch (type)
    {
    case CUBE:
        this->CreateCube();
        break;
    case SPHERE:
        this->CreateSphere();
        break;
    case PLANE:
        this->CreatePlane();
        break;
    case CYLINDER:
        this->CreateCylinder();
        break;
    case CAPSULE:
        this->CreateCapsule();
        break;
    default:
        break;
    }
}

void Primitive::CreateCube()
{
    this->name = "Cube";

    std::array<Vertex, 8> vertices =
    {
        Vertex({ XMFLOAT3(-0.5f+0.5f, -0.5f, -0.5f), XMFLOAT4(Colors::White) }),
        Vertex({ XMFLOAT3(-0.5f + 0.5f, +0.5f, -0.5f), XMFLOAT4(Colors::Black) }),
        Vertex({ XMFLOAT3(+0.5f + 0.5f, +0.5f, -0.5f), XMFLOAT4(Colors::Red) }),
        Vertex({ XMFLOAT3(+0.5f + 0.5f, -0.5f, -0.5f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(-0.5f + 0.5f, -0.5f, +0.5f), XMFLOAT4(Colors::Blue) }),
        Vertex({ XMFLOAT3(-0.5f + 0.5f, +0.5f, +0.5f), XMFLOAT4(Colors::Yellow) }),
        Vertex({ XMFLOAT3(+0.5f + 0.5f, +0.5f, +0.5f), XMFLOAT4(Colors::Cyan) }),
        Vertex({ XMFLOAT3(+0.5f + 0.5f, -0.5f, +0.5f), XMFLOAT4(Colors::Magenta) })
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

    vertexBuffer.Create(L"vertex buff", vertices.size(), sizeof(Vertex), vertices.data());
    indexBuffer.Create(L"index buff", indices.size(), sizeof(std::uint16_t), indices.data());
}

void Primitive::CreateSphere()
{
    this->name = "Sphere";

//    int rings = 32;
//    int sectors = 32;
//    float radius = 1.0f;
//
//    std::vector<unsigned int> indices;
//    std::vector<Vertex> vertices;
//
//    // Precomputed values to optimize calculations
//    float const ringStep = 1.0f / float(rings - 1);    // Step size for the rings
//    float const sectorStep = 1.0f / float(sectors - 1); // Step size for the sectors
//    float half = (float)rings / 2;
//
//    // Calculate vertices.
//    for (int ring = 0; ring < rings; ++ring)
//    {
//        for (unsigned int sector = 0; sector < sectors; ++sector)
//        {
//            // Calculate the spherical coordinates for the vertex
//            float y = sinf(-XM_PIDIV2 + XM_PI * ring * ringStep); // Vertical (Y) position
//            float x = cosf(2 * XM_PI * sector * sectorStep) * sinf(XM_PI * ring * ringStep); // Horizontal (X) position
//            float z = sinf(2 * XM_PI * sector * sectorStep) * sinf(XM_PI * ring * ringStep); // Depth (Z) position
//
//            XMFLOAT3 pos(x * radius, y * radius, z * radius);
//            XMFLOAT2 uv(sector * sectorStep, ring * ringStep);
//            XMFLOAT4 color = XMFLOAT4(Colors::White);
//
//            // Create the vertex with position, normal, and texture coordinate
//            Vertex vertex(pos, color);
//
//            // Store the vertex in the vertex buffer
//            vertices.push_back(vertex);
//        }
//    }
//
//    // Calculate indices.
//    int index = 0;
//    for (unsigned int ring = 0; ring < rings - 1; ++ring)
//    {
//        for (unsigned int sector = 0; sector < sectors - 1; ++sector)
//        {
//            // Define two triangles for each quad on the sphere's surface
//            // First triangle
//            indices.push_back(ring * sectors + sector);
//            indices.push_back((ring + 1) * sectors + sector);
//            indices.push_back((ring + 1) * sectors + (sector + 1));
//
//            // Second triangle
//            indices.push_back(ring * sectors + sector);
//            indices.push_back((ring + 1) * sectors + (sector + 1));
//            indices.push_back(ring * sectors + (sector + 1));
//        }
//    }
//
//    vertexBuffer.Create(L"vertex buff", vertices.size(), sizeof(Vertex), vertices.data());
//    indexBuffer.Create(L"index buff", indices.size(), sizeof(std::uint16_t), indices.data());
}

void Primitive::CreatePlane()
{
      this->name = "Plane";
//    std::array<Vertex, 8> vertices =
//    {
//        Vertex({ XMFLOAT3(-0.5f - 0.5f, -0.5f, -0.0f), XMFLOAT4(Colors::White) }),
//        Vertex({ XMFLOAT3(-0.5f - 0.5f, +0.5f, -0.0f), XMFLOAT4(Colors::Black) }),
//        Vertex({ XMFLOAT3(+0.5f - 0.5f, +0.5f, -0.0f), XMFLOAT4(Colors::Red) }),
//        Vertex({ XMFLOAT3(+0.5f - 0.5f, -0.5f, -0.0f), XMFLOAT4(Colors::Green) }),
//        Vertex({ XMFLOAT3(-0.5f - 0.5f, -0.5f, +0.0f), XMFLOAT4(Colors::Blue) }),
//        Vertex({ XMFLOAT3(-0.5f - 0.5f, +0.5f, +0.0f), XMFLOAT4(Colors::Yellow) }),
//        Vertex({ XMFLOAT3(+0.5f - 0.5f, +0.5f, +0.0f), XMFLOAT4(Colors::Cyan) }),
//        Vertex({ XMFLOAT3(+0.5f - 0.5f, -0.5f, +0.0f), XMFLOAT4(Colors::Magenta) })
//    };
//
//    std::array<std::uint16_t, 36> indices =
//    {
//        // front face
//        0, 1, 2,
//        0, 2, 3,
//
//        // back face
//        4, 6, 5,
//        4, 7, 6,
//    };
//
//    vertexBuffer.Create(L"vertex buff", vertices.size(), sizeof(Vertex), vertices.data());
//    indexBuffer.Create(L"index buff", indices.size(), sizeof(std::uint16_t), indices.data());
}

void Primitive::CreateCylinder()
{
    this->name = "Cylinder";
}

void Primitive::CreateCapsule()
{
    this->name = "Capsule";
//    std::array<Vertex, 8> vertices =
//    {
//        Vertex({ XMFLOAT3(-0.5f - 0.5f, -0.5f, -0.0f), XMFLOAT4(Colors::White) }),
//        Vertex({ XMFLOAT3(-0.5f - 0.5f, +0.5f, -0.0f), XMFLOAT4(Colors::Black) }),
//        Vertex({ XMFLOAT3(+0.5f - 0.5f, +0.5f, -0.0f), XMFLOAT4(Colors::Red) }),
//        Vertex({ XMFLOAT3(+0.5f - 0.5f, -0.5f, -0.0f), XMFLOAT4(Colors::Green) }),
//        Vertex({ XMFLOAT3(-0.5f - 0.5f, -0.5f, +0.0f), XMFLOAT4(Colors::Blue) }),
//        Vertex({ XMFLOAT3(-0.5f - 0.5f, +0.5f, +0.0f), XMFLOAT4(Colors::Yellow) }),
//        Vertex({ XMFLOAT3(+0.5f - 0.5f, +0.5f, +0.0f), XMFLOAT4(Colors::Cyan) }),
//        Vertex({ XMFLOAT3(+0.5f - 0.5f, -0.5f, +0.0f), XMFLOAT4(Colors::Magenta) })
//    };
//
//    std::array<std::uint16_t, 36> indices =
//    {
//        // front face
//        0, 1, 2,
//        0, 2, 3,
//
//        // back face
//        4, 6, 5,
//        4, 7, 6,
//    };
//
//    vertexBuffer.Create(L"vertex buff", vertices.size(), sizeof(Vertex), vertices.data());
//    indexBuffer.Create(L"index buff", indices.size(), sizeof(std::uint16_t), indices.data());
}

void Primitive::update(float deltaTime, RECT viewport)
{
    /*if (!rendered)
    {
        std::unique_ptr<GeometricPrimitive> cylinder = GeometricPrimitive::CreateCylinder();
        cylinder->Draw(context.GetCommandList());
        rendered = true;
    }*/
}

void Primitive::draw(GraphicsContext& context, Matrix4 viewMat)
{
    context.SetPipelineState(graphicsPSO);
    context.SetRootSignature(Renderer::m_RootSig);
    context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, Renderer::s_TextureHeap.GetHeapPointer());
    context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context.SetIndexBuffer(indexBuffer.IndexBufferView());
    context.SetVertexBuffer(0, vertexBuffer.VertexBufferView());

    context.SetDynamicConstantBufferView(0, sizeof(viewMat), &viewMat);

    context.DrawIndexedInstanced(36, 1, 0, 0, 0);

    context.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_PRESENT);
}
