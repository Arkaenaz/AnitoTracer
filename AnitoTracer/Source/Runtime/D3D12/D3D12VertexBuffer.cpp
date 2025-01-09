#include "AnitoTracerPCH.h"
#include "D3D12VertexBuffer.h"

namespace Anito
{
	D3D12VertexBuffer::D3D12VertexBuffer(D3D12RenderSystem* system) : system(system)
	{
	}

	D3D12VertexBuffer::~D3D12VertexBuffer()
	{
        this->vertexBuffer->Release();
	}

	void D3D12VertexBuffer::load(void* listVertices, UINT sizeVertex, UINT sizeList)
	{
        this->vertexBufferSize = sizeVertex * sizeList;

        D3D12_RESOURCE_DESC1 resourceDesc {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        resourceDesc.Width = this->vertexBufferSize;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        // TODO : Readd back when you know how to copy over buffers

        /*D3D12_RESOURCE_DESC1 rdu{};
        rdu.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        rdu.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        rdu.Width = this->vertexBufferSize;
        rdu.Height = 1;
        rdu.DepthOrArraySize = 1;
        rdu.MipLevels = 1;
        rdu.Format = DXGI_FORMAT_UNKNOWN;
        rdu.SampleDesc.Count = 1;
        rdu.SampleDesc.Quality = 0;
        rdu.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        rdu.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_HEAP_PROPERTIES defaultHeap {};
        defaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;
        defaultHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        defaultHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        defaultHeap.CreationNodeMask = 0;
        defaultHeap.VisibleNodeMask = 0;*/

        D3D12_HEAP_PROPERTIES uploadHeap{};
        uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        uploadHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadHeap.CreationNodeMask = 0;
        uploadHeap.VisibleNodeMask = 0;

        HRESULT hr = this->system->getDXContext()->getDevice()->CreateCommittedResource3(
            &uploadHeap, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED,
            nullptr, nullptr, 0, nullptr, IID_PPV_ARGS(&this->vertexBuffer));

        if (SUCCEEDED(hr))
        {
            Logger::debug(this, "Vertex Buffer created successfully");
        }
        else
        {
            Logger::error(this, "Vertex Buffer not created successfully");
        }

        // TODO : Readd upload buffer when you know how to make it

        /*hr = this->system->getDXContext()->getDevice()->CreateCommittedResource3(
            &uploadHeap, D3D12_HEAP_FLAG_NONE, &rdu, D3D12_BARRIER_LAYOUT_UNDEFINED,
            nullptr, nullptr, 0, nullptr,
            IID_PPV_ARGS(&this->uploadBuffer));

        if (SUCCEEDED(hr))
        {
            Logger::debug(this, "Upload Buffer created successfully");
        }
        else
        {
            Logger::error(this, "Upload Buffer not created successfully");
        }

        char* uploadBufferAddress;
        D3D12_RANGE uploadRange;
        uploadRange.Begin = 0;
        uploadRange.End = this->vertexBufferSize;
        uploadBuffer->Map(0, &uploadRange, (void**)&uploadBufferAddress);
        memcpy(&uploadBufferAddress[0], listVertices, sizeof(listVertices));
        uploadBuffer->Unmap(0, nullptr);

        this->system->getDXContext()->copyBufferRegion(vertexBuffer, 0, uploadBuffer, 0, this->vertexBufferSize);*/

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        hr = this->vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
        if (SUCCEEDED(hr))
        {
            Logger::debug(this, "Mapping Vertex Buffer Succeeded");
        }
        else
        {
            Logger::debug(this, "Mapping Vertex Buffer Failed");
            Logger::logHResult(this, hr);
        }
        memcpy(pVertexDataBegin, listVertices, this->vertexBufferSize);
        this->vertexBuffer->Unmap(0, nullptr);

        this->vertexBufferView.BufferLocation = this->vertexBuffer->GetGPUVirtualAddress();
        this->vertexBufferView.StrideInBytes = sizeVertex;
        this->vertexBufferView.SizeInBytes = this->vertexBufferSize;

        //this->uploadBuffer->Release();
	}

	D3D12_VERTEX_BUFFER_VIEW D3D12VertexBuffer::getVertexBufferView()
	{
        return this->vertexBufferView;
	}
}

