#include "AnitoTracerPCH.h"
#include "D3D12Buffer.h"

namespace Anito
{
	D3D12Buffer* D3D12Buffer::createVertexBuffer(const D3D12Device& device, UINT size)
	{
		D3D12_RESOURCE_DESC1 resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDesc.Width = size;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		/*D3D12_HEAP_PROPERTIES defaultHeap{};
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

		ID3D12Resource2* buffer = nullptr;

		HRESULT hr = device.get()->CreateCommittedResource3(
			&uploadHeap, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED,
			nullptr, nullptr, 0, nullptr, IID_PPV_ARGS(&buffer));

		if (SUCCEEDED(hr))
		{
			Logger::debug("Vertex Buffer created successfully");
		}
		else
		{
			Logger::error("Vertex Buffer not created successfully");
		}

		return new D3D12Buffer(buffer);
	}

	D3D12Buffer* D3D12Buffer::createIndexBuffer(const D3D12Device& device, UINT size)
	{
		D3D12_RESOURCE_DESC1 resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDesc.Width = size;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		/*D3D12_HEAP_PROPERTIES defaultHeap{};
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

		ID3D12Resource2* buffer = nullptr;

		HRESULT hr = device.get()->CreateCommittedResource3(
			&uploadHeap, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED,
			nullptr, nullptr, 0, nullptr, IID_PPV_ARGS(&buffer));

		if (SUCCEEDED(hr))
		{
			Logger::debug("Index Buffer created successfully");
		}
		else
		{
			Logger::error("Index Buffer not created successfully");
		}

		return new D3D12Buffer(buffer);
	}

	/*D3D12Buffer* D3D12Buffer::createUploadBuffer(const D3D12Device& device, UINT size)
	{
		D3D12_RESOURCE_DESC1 resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDesc.Width = size;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES uploadHeap{};
		uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeap.CreationNodeMask = 0;
		uploadHeap.VisibleNodeMask = 0;

		ID3D12Resource2* buffer;

		HRESULT hr = device.get()->CreateCommittedResource2(
			&uploadHeap, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, nullptr, IID_PPV_ARGS(&buffer));

		if (SUCCEEDED(hr))
		{
			Logger::debug("Upload Buffer created successfully");
		}
		else
		{
			Logger::error("Upload Buffer not created successfully");
		}

		return new D3D12Buffer(buffer);
	}*/

	D3D12Buffer::D3D12Buffer(ID3D12Resource2* resource) : D3D12Resource(resource)
	{
		this->gpuVirtualAddress = this->resource->GetGPUVirtualAddress();
	}

	D3D12Buffer::~D3D12Buffer()
	{
		D3D12Resource::~D3D12Resource();
	}

	void D3D12Buffer::copyFromCPU(const void* data, UINT size) const
	{
		char* bufferData = nullptr;
		CD3DX12_RANGE uploadRange;
		this->resource->Map(0, &uploadRange, reinterpret_cast<void**>(&bufferData));
		memcpy(&bufferData[0], data, size);
		this->resource->Unmap(0, nullptr);
	}

	void D3D12Buffer::copyFromBuffer(D3D12CommandContext* context, const D3D12Buffer& buffer) const
	{
		context->copyBufferRegion(this->resource, 0, buffer.get(), 0,
			this->resource->GetDesc1().Width);
	}
}
