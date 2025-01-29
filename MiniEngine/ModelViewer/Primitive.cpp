#include "pch.h"
#include "Primitive.h"

#include <array>
#include <d3dcompiler.h>
#include <filesystem>
#include <DirectXColors.h>

#include "Renderer.h"
#include "CommandContext.h"

#include "ConstantBuffers.h"

using namespace DirectX;
using namespace Graphics;
using namespace Math;
using namespace Renderer;

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

    graphicsPSO.SetRootSignature(Renderer::m_RootSig);
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

    m_BoundingSphere = BoundingSphere(kZero);
    m_DataBuffer.Destroy();
    m_NumNodes = 0;
    m_NumMeshes = 0;
    m_MeshData = nullptr;
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

    this->m_NumNodes = 1;
    this->m_SceneGraph.reset(new GraphNode[vertices.size()]);
    this->m_NumMeshes = 1;
    this->m_MeshData.reset(new uint8_t[indices.size() * sizeof(Vertex)]);
	this->m_DataBuffer.Create(L"Model Data", vertices.size(), sizeof(Vertex), vertices.data());
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

void Primitive::Update(float deltaTime, RECT viewport)
{
    /*if (!rendered)
    {
        std::unique_ptr<GeometricPrimitive> cylinder = GeometricPrimitive::CreateCylinder();
        cylinder->Draw(context.GetCommandList());
        rendered = true;
    }*/
}

void Primitive::Render(GraphicsContext& context, Matrix4 viewMat)
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

void Primitive::Render(Renderer::MeshSorter& sorter, const GpuBuffer& Constants,
	const AffineTransform sphereTransforms[]) const
{
    // Pointer to current mesh
    const uint8_t* pMesh = m_MeshData.get();

    const Frustum& frustum = sorter.GetViewFrustum();
    const AffineTransform& viewMat = (const AffineTransform&)sorter.GetViewMatrix();

    for (uint32_t i = 0; i < m_NumMeshes; ++i)
    {
        const Mesh& mesh = *(const Mesh*)pMesh;

        const AffineTransform& sphereXform = sphereTransforms[mesh.meshCBV];
        Scalar scaleXSqr = LengthSquare(sphereXform.GetX());
        Scalar scaleYSqr = LengthSquare(sphereXform.GetY());
        Scalar scaleZSqr = LengthSquare(sphereXform.GetZ());
        Scalar sphereScale = Sqrt(Max(Max(scaleXSqr, scaleYSqr), scaleZSqr));

        BoundingSphere sphereLS((const XMFLOAT4*)mesh.bounds);
        BoundingSphere sphereWS = BoundingSphere(sphereXform * sphereLS.GetCenter(), sphereScale * sphereLS.GetRadius());
        BoundingSphere sphereVS = BoundingSphere(viewMat * sphereWS.GetCenter(), sphereWS.GetRadius());

        if (frustum.IntersectSphere(sphereVS))
        {
            float distance = -sphereVS.GetCenter().GetZ() - sphereVS.GetRadius();
            sorter.AddMesh(mesh, distance,
                Constants.GetGpuVirtualAddress() + sizeof(MeshConstants) * mesh.meshCBV,
                m_DataBuffer.GetGpuVirtualAddress());
        }

        pMesh += sizeof(Mesh) + (mesh.numDraws - 1) * sizeof(Mesh::Draw);
    }
}

PrimitiveInstance::PrimitiveInstance(std::shared_ptr<const Primitive> sourcePrimitive)
    : m_Primitive(sourcePrimitive), m_Locator(kIdentity)
{
    static_assert((_alignof(Constants) & 255) == 0, "CBVs need 256 byte alignment");
    if (sourcePrimitive == nullptr)
    {
        m_ConstantsCPU.Destroy();
        m_ConstantsGPU.Destroy();
        m_BoundingSphereTransforms = nullptr;
    }
    else
    {
        m_ConstantsCPU.Create(L"Mesh Constant Upload Buffer", sourcePrimitive->m_NumNodes * sizeof(Constants));
        m_ConstantsGPU.Create(L"Mesh Constant GPU Buffer", sourcePrimitive->m_NumNodes, sizeof(Constants));

        m_BoundingSphereTransforms.reset(new AffineTransform[sourcePrimitive->m_NumNodes]);
    }
}

PrimitiveInstance::PrimitiveInstance(const PrimitiveInstance& primInstance)
	: PrimitiveInstance(primInstance.m_Primitive) {}

PrimitiveInstance& PrimitiveInstance::operator=(std::shared_ptr<const Primitive> sourcePrimitive)
{
    m_Primitive = sourcePrimitive;
    m_Locator = UniformTransform(kIdentity);
    if (sourcePrimitive == nullptr)
    {
        m_ConstantsCPU.Destroy();
        m_ConstantsGPU.Destroy();
        m_BoundingSphereTransforms = nullptr;
    }
    else
    {
        m_ConstantsCPU.Create(L"Mesh Constant Upload Buffer", sourcePrimitive->m_NumNodes * sizeof(Constants));
        m_ConstantsGPU.Create(L"Mesh Constant GPU Buffer", sourcePrimitive->m_NumNodes, sizeof(Constants));

        m_BoundingSphereTransforms.reset(new AffineTransform[sourcePrimitive->m_NumNodes]);
    }
    return *this;
}

void PrimitiveInstance::Update(GraphicsContext& gfxContext, float deltaTime)
{
    if (m_Primitive == nullptr)
        return;

    static const size_t kMaxStackDepth = 32;
    size_t stackIdx = 0;
    Matrix4 matrixStack[kMaxStackDepth];
    Matrix4 ParentMatrix = Matrix4((AffineTransform)m_Locator);

    Constants* cb = (Constants*)m_ConstantsCPU.Map();

    const GraphNode* sceneGraph = m_Primitive->m_SceneGraph.get();

    // Traverse the scene graph in depth first order.  This is the same as linear order
    // for how the nodes are stored in memory.  Uses a matrix stack instead of recursion.
    for (const GraphNode* Node = sceneGraph; ; ++Node)
    {
        Matrix4 xform = Node->xform;
        if (!Node->skeletonRoot)
            xform = ParentMatrix * xform;

        // Concatenate the transform with the parent's matrix and update the matrix list
        {
            // Scoped so that I don't forget that I'm pointing to write-combined memory and
            // should not read from it.
            Constants& cbv = cb[0];
            cbv.World = xform;
            cbv.WorldIT = InverseTranspose(xform.Get3x3());

            m_BoundingSphereTransforms[0] = AffineTransform(
                (Vector3)xform.GetX(),
                (Vector3)xform.GetY(),
                (Vector3)xform.GetZ(),
                (Vector3)xform.GetW());
        }

        // If the next node will be a descendent, replace the parent matrix with our new matrix
        if (Node->hasChildren)
        {
            // ...but if we have siblings, make sure to backup our current parent matrix on the stack
            if (Node->hasSibling)
            {
                ASSERT(stackIdx < kMaxStackDepth, "Overflowed the matrix stack");
                matrixStack[stackIdx++] = ParentMatrix;
            }
            ParentMatrix = xform;
        }
        else if (!Node->hasSibling)
        {
            // There are no more siblings.  If the stack is empty, we are done.  Otherwise, pop
            // a matrix off the stack and continue.
            if (stackIdx == 0)
                break;

            ParentMatrix = matrixStack[--stackIdx];
        }
    }

    m_ConstantsCPU.Unmap();

    gfxContext.TransitionResource(m_ConstantsGPU, D3D12_RESOURCE_STATE_COPY_DEST, true);
    gfxContext.GetCommandList()->CopyBufferRegion(m_ConstantsGPU.GetResource(), 0, m_ConstantsCPU.GetResource(), 0, m_ConstantsCPU.GetBufferSize());
    gfxContext.TransitionResource(m_ConstantsGPU, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void PrimitiveInstance::Render(Renderer::MeshSorter& sorter) const
{
    if (m_Primitive == nullptr)
        return;

    const Frustum& frustum = sorter.GetWorldFrustum();
    m_Primitive->Render(sorter, m_ConstantsGPU, m_BoundingSphereTransforms.get());
}

void PrimitiveInstance::Resize(float newRadius)
{
    if (m_Primitive == nullptr)
        return;

    m_Locator.SetScale(newRadius / m_Primitive->m_BoundingSphere.GetRadius());
}

Vector3 PrimitiveInstance::GetCenter() const
{
    if (m_Primitive == nullptr)
        return Vector3(kOrigin);

    return m_Locator * m_Primitive->m_BoundingSphere.GetCenter();
}

Scalar PrimitiveInstance::GetRadius() const
{
    if (m_Primitive == nullptr)
        return Scalar(kZero);

    return m_Locator.GetScale() * m_Primitive->m_BoundingSphere.GetRadius();
}

BoundingSphere PrimitiveInstance::GetBoundingSphere() const
{
    if (m_Primitive == nullptr)
        return BoundingSphere(kZero);

    return m_Locator * m_Primitive->m_BoundingSphere;
}

OrientedBox PrimitiveInstance::GetBoundingBox() const
{
    if (m_Primitive == nullptr)
        return AxisAlignedBox(Vector3(kZero), Vector3(kZero));

    return m_Locator * m_Primitive->m_BoundingBox;
}
