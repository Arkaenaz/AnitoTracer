#pragma once

#include "Window.h"
#include "Dx12/InterfacePointers.h"
#include "_externals/glm/glm/glm.hpp"
#include <vector>

#include "imgui.h"

#include "Utils/D3D12GraphicsContext.h"
#include "Utils/Structs/AccelerationStructureBuffers.h"
#include "Utils/Structs/FrameObject.h"
#include "Utils/Structs/HeapData.h"

using Microsoft::WRL::ComPtr;

class AppWindow final : public Window
{
public:
	// Simple free list based allocator
	struct ExampleDescriptorHeapAllocator
	{
		ID3D12DescriptorHeap* Heap = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
		D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
		D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
		UINT                        HeapHandleIncrement;
		ImVector<int>               FreeIndices;

		void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
		{
			IM_ASSERT(Heap == nullptr && FreeIndices.empty());
			Heap = heap;
			D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
			HeapType = desc.Type;
			HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
			HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
			HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
			FreeIndices.reserve((int)desc.NumDescriptors);
			for (int n = desc.NumDescriptors; n > 0; n--)
				FreeIndices.push_back(n);
		}
		void Destroy()
		{
			Heap = nullptr;
			FreeIndices.clear();
		}
		void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
		{
			IM_ASSERT(FreeIndices.Size > 0);
			int idx = FreeIndices.back();
			FreeIndices.pop_back();
			out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
			out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
		}
		void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
		{
			int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
			int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
			IM_ASSERT(cpu_idx == gpu_idx);
			FreeIndices.push_back(cpu_idx);
		}
	};

public:
	AppWindow(UINT width, UINT height, std::wstring name);

	void OnInit() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnDestroy() override;

	static SampleFramework::ID3D12RootSignaturePtr createRootSignature(
		SampleFramework::ID3D12Device5Ptr pDevice,
		const D3D12_ROOT_SIGNATURE_DESC& desc
	);

private:
	SampleFramework::ID3D12Device5Ptr mpDevice;
	SampleFramework::ID3D12CommandQueuePtr mpCmdQueue;
	SampleFramework::IDXGISwapChain3Ptr mpSwapChain;
	glm::uvec2 mSwapChainSize;
	SampleFramework::ID3D12GraphicsCommandList4Ptr mpCmdList;
	SampleFramework::ID3D12FencePtr mpFence;
	HANDLE mFenceEvent;
	uint64_t mFenceValue = 0;

	std::vector<DirectXUtil::Structs::FrameObject> mFrameObjects;

	DirectXUtil::Structs::HeapData mRtvHeap;
	static const uint32_t kRtvHeapSize = 3;

	// imgui
	static ExampleDescriptorHeapAllocator g_pd3dSrvDescHeapAlloc;
	void InitializeGUI();


	//-----------------------------------------------
	// DXR HELPER STUFF
	//-----------------------------------------------

	DirectXUtil::D3D12GraphicsContext D3D12UtilContext;

	uint32_t BeginFrame() const;
	void EndFrame(uint32_t rtvIndex);

	//-----------------------------------------------
	// DXR FUNCTIONS
	//-----------------------------------------------

	void createAccelerationStructures();
	std::vector<DirectXUtil::Structs::AccelerationStructureBuffers> mpBottomLevelAS;
	DirectXUtil::Structs::AccelerationStructureBuffers mTopLevelBuffers;
	uint64_t mTlasSize = 0;

	void createRtPipelineState();
	SampleFramework::ID3D12StateObjectPtr mpPipelineState;
	SampleFramework::ID3D12RootSignaturePtr mpEmptyRootSig;

	void createShaderTable();
	SampleFramework::ID3D12ResourcePtr mpShaderTable;
	uint32_t mShaderTableEntrySize = 0;

	void createShaderResources();
	SampleFramework::ID3D12ResourcePtr mpOutputResource;
	SampleFramework::ID3D12DescriptorHeapPtr mpSrvUavHeap;
	static const uint32_t kSrvUavHeapSize = 2;

	//-----------------------------------------------
	// DXR PRIMITIVE
	//-----------------------------------------------

	D3D12_CPU_DESCRIPTOR_HANDLE indexSRVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE vertexSRVHandle;
};
