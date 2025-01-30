#include "AppWindow.h"

#include "Utils.h"
#include "Win32Application.h"
#include "_externals/glm/glm/gtx/transform.hpp"
#include "_externals/glm/glm/gtx/euler_angles.hpp"
#include <array>
#include "Utils/AccelerationStructures.h"
#include "Utils/RTPipeline.h"
#include "Utils/Structs/ExportAssociation.h"
#include "Utils/Structs/GlobalRootSignature.h"
#include "Utils/Structs/HitProgram.h"
#include "Utils/Structs/LocalRootSignature.h"
#include "Utils/Structs/PipelineConfig.h"
#include "Utils/Structs/ShaderConfig.h"
#include "Utils/Structs/VertexPositionNormalTangentTexture.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                      FenceValue;
};

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

// Data
// static FrameContext                 g_frameContext[3] = {};
 //static UINT                         g_frameIndex = 0;
//
// static ID3D12Device* g_pd3dDevice = nullptr;
// static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = nullptr;
// static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = nullptr;
// static ExampleDescriptorHeapAllocator g_pd3dSrvDescHeapAlloc;
// static ID3D12CommandQueue* g_pd3dCommandQueue = nullptr;
// static ID3D12GraphicsCommandList* mpCmdList = nullptr;
// static ID3D12Fence* g_fence = nullptr;
// static HANDLE                       g_fenceEvent = nullptr;
// static UINT64                       g_fenceLastSignaledValue = 0;
// static IDXGISwapChain3* g_pSwapChain = nullptr;
// static bool                         g_SwapChainOccluded = false;
// static HANDLE                       g_hSwapChainWaitableObject = nullptr;
// static ID3D12Resource* g_mainRenderTargetResource[2] = {};
// static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[2] = {};

// FrameContext* WaitForNextFrameResources()
// {
// 	UINT nextFrameIndex = g_frameIndex + 1;
// 	g_frameIndex = nextFrameIndex;
//
// 	HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, nullptr };
// 	DWORD numWaitableObjects = 1;
//
// 	FrameContext* frameCtx = &g_frameContext[nextFrameIndex % 3];
// 	UINT64 fenceValue = frameCtx->FenceValue;
// 	if (fenceValue != 0) // means no fence was signaled
// 	{
// 		frameCtx->FenceValue = 0;
// 		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
// 		waitableObjects[1] = g_fenceEvent;
// 		numWaitableObjects = 2;
// 	}
//
// 	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);
//
// 	return frameCtx;
// }

AppWindow::AppWindow(const UINT width, const UINT height,
	const std::wstring name)
	: Window(width, height, name)
{
	mSwapChainSize = glm::uvec2(GetWidth(), GetHeight());
}

void AppWindow::OnInit()
{
	const HWND mHwnd = Win32Application::GetHwnd();
	const UINT winWidth = GetWidth();
	const UINT winHeight = GetHeight();

	// Debug Layer
#ifdef _DEBUG
	SampleFramework::ID3D12Debug1Ptr pDx12Debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDx12Debug))))
	{
		pDx12Debug->SetEnableGPUBasedValidation(true);
		pDx12Debug->SetEnableSynchronizedCommandQueueValidation(true);
		pDx12Debug->EnableDebugLayer();
	}

#endif

	// Create the DXGI factory
	SampleFramework::IDXGIFactory4Ptr pDxgiFactory;
	NV_D3D_CALL(CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory)));
	mpDevice = D3D12UtilContext.createDevice(pDxgiFactory);
	mpCmdQueue = D3D12UtilContext.createCommandQueue(mpDevice);
	mpSwapChain = D3D12UtilContext.createDxgiSwapChain(pDxgiFactory, mHwnd, winWidth, winHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM, mpCmdQueue);

	// Create a RTV descriptor heap
	mRtvHeap.pHeap = D3D12UtilContext.createDescriptorHeap(mpDevice, kRtvHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		false);

	// Create the per-frame objects
	mFrameObjects.resize(DirectXUtil::D3D12GraphicsContext::getDefaultSwapChainBuffers());
	for (uint32_t i = 0; i < mFrameObjects.size(); i++)
	{
		NV_D3D_CALL(
			mpDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(
					&mFrameObjects[i].pCmdAllocator
				)
			)
		);

		NV_D3D_CALL(
			mpSwapChain->GetBuffer(
				i,
				IID_PPV_ARGS(
					&mFrameObjects[i].pSwapChainBuffer
				)
			)
		);

		mFrameObjects[i].rtvHandle = DirectXUtil::D3D12GraphicsContext::createRTV(
			mpDevice,
			mFrameObjects[i].pSwapChainBuffer,
			mRtvHeap.pHeap,
			mRtvHeap.usedEntries,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
		);
	}

	// Create the command-list
	NV_D3D_CALL(
		mpDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			mFrameObjects[0].pCmdAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(&mpCmdList)
		)
	);

	// Create a fence and the event
	NV_D3D_CALL(mpDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpFence)));
	mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	createAccelerationStructures();

	createRtPipelineState();

	createShaderResources();

	createShaderTable();

	InitializeGUI();
}

void AppWindow::OnUpdate()
{
	//Not used in this tutorial
}

void AppWindow::OnRender()
{
	//OutputDebugString(std::to_string(mpCmdQueue ));
	const uint32_t rtvIndex = BeginFrame();

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Let's raytrace
	DirectXUtil::D3D12GraphicsContext::resourceBarrier(
		mpCmdList,
		mpOutputResource,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	D3D12_DISPATCH_RAYS_DESC raytraceDesc = {};
	raytraceDesc.Width = mSwapChainSize.x;
	raytraceDesc.Height = mSwapChainSize.y;
	raytraceDesc.Depth = 1;

	// RayGen is the first entry in the shader-table
	raytraceDesc.RayGenerationShaderRecord.StartAddress =
		mpShaderTable->GetGPUVirtualAddress() + 0 * mShaderTableEntrySize;
	raytraceDesc.RayGenerationShaderRecord.SizeInBytes = mShaderTableEntrySize;

	// Miss is the second entry in the shader-table
	const size_t missOffset = 1 * mShaderTableEntrySize;
	raytraceDesc.MissShaderTable.StartAddress = mpShaderTable->GetGPUVirtualAddress() + missOffset;
	raytraceDesc.MissShaderTable.StrideInBytes = mShaderTableEntrySize;
	raytraceDesc.MissShaderTable.SizeInBytes = mShaderTableEntrySize * 2;

	// Hit is the fourth entry in the shader-table
	const size_t hitOffset = 3 * mShaderTableEntrySize;
	raytraceDesc.HitGroupTable.StartAddress = mpShaderTable->GetGPUVirtualAddress() + hitOffset;
	raytraceDesc.HitGroupTable.StrideInBytes = mShaderTableEntrySize;
	raytraceDesc.HitGroupTable.SizeInBytes = mShaderTableEntrySize * 1;

	// Bind the empty root signature
	mpCmdList->SetComputeRootSignature(mpEmptyRootSig.Get());

	// Dispatch
	mpCmdList->SetPipelineState1(mpPipelineState.Get());
	mpCmdList->DispatchRays(&raytraceDesc);

	// Copy the results to the back-buffer
	DirectXUtil::D3D12GraphicsContext::resourceBarrier(
		mpCmdList,
		mpOutputResource,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	DirectXUtil::D3D12GraphicsContext::resourceBarrier(
		mpCmdList,
		mFrameObjects[rtvIndex].pSwapChainBuffer,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	mpCmdList->CopyResource(mFrameObjects[rtvIndex].pSwapChainBuffer.Get(), mpOutputResource.Get());

	ImGui::ShowDemoWindow();

	// Rendering
	ImGui::Render();

	// FrameContext* frameCtx = WaitForNextFrameResources();
	// UINT backBufferIdx = mpSwapChain->GetCurrentBackBufferIndex();
	// frameCtx->CommandAllocator->Reset();

	//mFrameObjects[rtvIndex].pCmdAllocator.Get()->Reset();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFrameObjects[rtvIndex].pSwapChainBuffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//mpCmdList->Reset(mFrameObjects[rtvIndex].pCmdAllocator.Get(), nullptr);
	mpCmdList->ResourceBarrier(1, &barrier);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRtvHeap.pHeap->GetCPUDescriptorHandleForHeapStart();

	// Render Dear ImGui graphics
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	mpCmdList->ClearRenderTargetView(mFrameObjects[rtvIndex].rtvHandle, clear_color_with_alpha, 0, nullptr);
	mpCmdList->OMSetRenderTargets(1, &mFrameObjects[rtvIndex].rtvHandle, FALSE, nullptr);
	mpCmdList->SetDescriptorHeaps(1, &mpSrvUavHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mpCmdList.Get());
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mpCmdList->ResourceBarrier(1, &barrier);
	mpCmdList->Close();

	mpCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)mpCmdList.Get());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	// Present
	HRESULT hr = mpSwapChain->Present(1, 0);   // Present with vsync
	//HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
	//g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

	mpCmdList->Close();
	ID3D12CommandList* pGraphicsList = mpCmdList.Get();
	mpCmdQueue->ExecuteCommandLists(1, &pGraphicsList);
	mFenceValue++;
	mpCmdQueue->Signal(mpFence.Get(), mFenceValue);

	// UINT64 fenceValue = g_fenceLastSignaledValue + 1;
	// mpCmdQueue->Signal(g_fence, fenceValue);
	// g_fenceLastSignaledValue = fenceValue;
	// frameCtx->FenceValue = fenceValue;

	//EndFrame(rtvIndex);
}

void AppWindow::OnDestroy()
{
	// Wait for the command queue to finish execution
	mFenceValue++;
	mpCmdQueue->Signal(mpFence.Get(), mFenceValue);
	mpFence->SetEventOnCompletion(mFenceValue, mFenceEvent);
	WaitForSingleObject(mFenceEvent, INFINITE);
}

AppWindow::ExampleDescriptorHeapAllocator AppWindow::g_pd3dSrvDescHeapAlloc;

void AppWindow::InitializeGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	io.Fonts->AddFontDefault();

	// D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle = nullptr;
	// D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle = nullptr;
	// g_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle);

	ImGui_ImplWin32_Init(Win32Application::GetHwnd());

	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = mpDevice.Get();
	init_info.CommandQueue = mpCmdQueue.Get();
	init_info.NumFramesInFlight = 3;
	init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
	// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
	// (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
	init_info.SrvDescriptorHeap = mpSrvUavHeap.Get();
	init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return g_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle); };
	init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) { return g_pd3dSrvDescHeapAlloc.Free(cpu_handle, gpu_handle); };
	ImGui_ImplDX12_Init(&init_info);

	// ImGui_ImplDX12_Init(
	// 	mpDevice.Get(),
	// 	// Number of frames in flight.
	// 	3,
	// 	DXGI_FORMAT_R8G8B8A8_UNORM,
	// 	// imgui needs SRV descriptors for its font textures.
	// 	mpSrvUavHeap.Get(),
	// 	mpSrvUavHeap.Get()->GetCPUDescriptorHandleForHeapStart(),
	// 	mpSrvUavHeap.Get()->GetGPUDescriptorHandleForHeapStart()
	// 	);
}

uint32_t AppWindow::BeginFrame() const
{
	// Bind the descriptor heaps
	ID3D12DescriptorHeap* heaps[] = { mpSrvUavHeap.Get() };
	mpCmdList->SetDescriptorHeaps(NV_ARRAYSIZE(heaps), heaps);
	return mpSwapChain->GetCurrentBackBufferIndex();
}

void AppWindow::EndFrame(const uint32_t rtvIndex)
{
	DirectXUtil::D3D12GraphicsContext::resourceBarrier(
		mpCmdList,
		mFrameObjects[rtvIndex].pSwapChainBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PRESENT
	);
	mFenceValue = DirectXUtil::D3D12GraphicsContext::submitCommandList(mpCmdList, mpCmdQueue, mpFence, mFenceValue);
	mpSwapChain->Present(0, 0);

	// Prepare the command list for the next frame
	const uint32_t bufferIndex = mpSwapChain->GetCurrentBackBufferIndex();

	// Make sure we have the new back-buffer is ready
	const uint32_t kDefaultSwapChainBuffers = DirectXUtil::D3D12GraphicsContext::getDefaultSwapChainBuffers();
	if (mFenceValue > kDefaultSwapChainBuffers)
	{
		mpFence->SetEventOnCompletion(mFenceValue - kDefaultSwapChainBuffers + 1, mFenceEvent);
		WaitForSingleObject(mFenceEvent, INFINITE);
	}

	mFrameObjects[bufferIndex].pCmdAllocator->Reset();
	mpCmdList->Reset(mFrameObjects[bufferIndex].pCmdAllocator.Get(), nullptr);
}

SampleFramework::ID3D12RootSignaturePtr AppWindow::createRootSignature(
	SampleFramework::ID3D12Device5Ptr pDevice, const D3D12_ROOT_SIGNATURE_DESC& desc)
{
	SampleFramework::ID3DBlobPtr pSigBlob;
	SampleFramework::ID3DBlobPtr pErrorBlob;
	const HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pSigBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		const std::string msg = convertBlobToString(pErrorBlob.Get());
		msgBox(msg);
		return nullptr;
	}
	SampleFramework::ID3D12RootSignaturePtr pRootSig;
	NV_D3D_CALL(
		pDevice->CreateRootSignature(
			0,
			pSigBlob->GetBufferPointer(),
			pSigBlob->GetBufferSize(),
			IID_PPV_ARGS(&pRootSig)
		)
	);

	return pRootSig;
}

void AppWindow::createAccelerationStructures()
{
	//Create the two bottom level AS
	mpBottomLevelAS.push_back(DirectXUtil::AccelerationStructures::createPlaneBottomLevelAS(
		mpDevice,
		mpCmdList));
	mpBottomLevelAS.push_back(DirectXUtil::AccelerationStructures::createPrimitiveBottomLevelAS(
		mpDevice,
		mpCmdList));

	mpBottomLevelAS.push_back(DirectXUtil::AccelerationStructures::createPrimitiveBottomLevelAS(
		mpDevice,
		mpCmdList));

	// Create the TLAS
	DirectXUtil::AccelerationStructures::buildTopLevelAS(
		mpDevice,
		mpCmdList,
		mpBottomLevelAS,
		mTlasSize,
		mTopLevelBuffers
	);

	// The tutorial doesn't have any resource lifetime management, so we flush and sync here.
	// This is not required by the DXR spec - you can submit the list whenever you like as long as you take care of the resources lifetime.
	mFenceValue = DirectXUtil::D3D12GraphicsContext::submitCommandList(mpCmdList, mpCmdQueue, mpFence, mFenceValue);

	mpFence->SetEventOnCompletion(mFenceValue, mFenceEvent);
	WaitForSingleObject(mFenceEvent, INFINITE);

	mpCmdList->Reset(mFrameObjects[0].pCmdAllocator.Get(), nullptr);
}

void AppWindow::createRtPipelineState()
{
	// Need 12 subobjects:
	//  1 for the DXIL library
	//  1 for hit-group
	//  2 for RayGen root-signature (root-signature and the subobject association)
	//  2 for hit-program root-signature (root-signature and the subobject association)
	//  2 for miss-shader root-signature (signature and association)
	//  2 for shader config (shared between all programs. 1 for the config, 1 for association)
	//  1 for pipeline config
	//  1 for the global root signature
	std::array<D3D12_STATE_SUBOBJECT, 12> subobjects{};
	uint32_t index = 0;

	// Create the DXIL library
	DirectXUtil::Structs::DxilLibrary dxilLib = DirectXUtil::RTPipeline::createDxilLibrary();
	subobjects[index++] = dxilLib.stateSubobject; // 0 Library

	DirectXUtil::Structs::HitProgram hitProgram(nullptr, DirectXUtil::RTPipeline::kClosestHitShader,
		DirectXUtil::RTPipeline::kHitGroup);
	subobjects[index++] = hitProgram.subObject; // 1 Hit Group

	// Create the ray-gen root-signature and association
	DirectXUtil::Structs::LocalRootSignature rgsRootSignature(
		mpDevice, DirectXUtil::RTPipeline::createRayGenRootDesc().desc);
	subobjects[index] = rgsRootSignature.subobject; // 2 RayGen Root Sig

	const uint32_t rgsRootIndex = index++; // 2
	DirectXUtil::Structs::ExportAssociation rgsRootAssociation(&DirectXUtil::RTPipeline::kRayGenShader, 1,
		&(subobjects[rgsRootIndex]));
	subobjects[index++] = rgsRootAssociation.subobject; // 3 Associate Root Sig to RGS

	// Create the hit root-signature and association
	DirectXUtil::Structs::LocalRootSignature hitRootSignature(
		mpDevice, DirectXUtil::RTPipeline::CreateHitRootDesc().desc);
	subobjects[index] = hitRootSignature.subobject; // 4 Hit Root Sig

	const uint32_t hitRootIndex = index++; // 4
	DirectXUtil::Structs::ExportAssociation hitRootAssociation(&DirectXUtil::RTPipeline::kClosestHitShader, 1,
		&(subobjects[hitRootIndex]));
	subobjects[index++] = hitRootAssociation.subobject; // 6 Associate Hit Root Sig to Hit Group

	// Create the miss root-signature and association
	D3D12_ROOT_SIGNATURE_DESC emptyDesc = {};
	emptyDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
	DirectXUtil::Structs::LocalRootSignature missRootSignature(mpDevice, emptyDesc);
	subobjects[index] = missRootSignature.subobject; // 6 Miss Root Sig

	const uint32_t missRootIndex = index++; // 6
	const WCHAR* missRootShaders[] = {
		DirectXUtil::RTPipeline::kMissShader,
		DirectXUtil::RTPipeline::kShadowMiss
	};
	DirectXUtil::Structs::ExportAssociation missRootAssociation(missRootShaders, NV_ARRAYSIZE(missRootShaders),
		&(subobjects[missRootIndex]));
	subobjects[index++] = missRootAssociation.subobject; // 7 Associate Miss Root Sig to Miss Shader

	// Bind the payload size to all programs
	//MaxPayloadSize float4 + uint
	DirectXUtil::Structs::ShaderConfig shaderConfig(sizeof(float) * 2, sizeof(float) * (4 + 1));
	subobjects[index] = shaderConfig.subobject; // 8 Shader Config;

	const uint32_t shaderConfigIndex = index++; // 8
	const WCHAR* shaderExports[] = {
		DirectXUtil::RTPipeline::kMissShader,
		DirectXUtil::RTPipeline::kClosestHitShader,
		DirectXUtil::RTPipeline::kRayGenShader,
		DirectXUtil::RTPipeline::kShadowMiss
	};
	DirectXUtil::Structs::ExportAssociation configAssociation(shaderExports, NV_ARRAYSIZE(shaderExports),
		&(subobjects[shaderConfigIndex]));
	subobjects[index++] = configAssociation.subobject; // 9 Associate Shader Config to Miss, CHS, RGS

	// Create the pipeline config
	DirectXUtil::Structs::PipelineConfig config(4 + 1);
	subobjects[index++] = config.subobject; // 10

	// Create the global root signature and store the empty signature
	DirectXUtil::Structs::GlobalRootSignature root(mpDevice, {});
	mpEmptyRootSig = root.pRootSig;
	subobjects[index++] = root.subobject; // 11

	// Create the state
	D3D12_STATE_OBJECT_DESC desc;
	desc.NumSubobjects = index;
	desc.pSubobjects = subobjects.data();
	desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;

	NV_D3D_CALL(mpDevice->CreateStateObject(&desc, IID_PPV_ARGS(&mpPipelineState)));
}

void AppWindow::createShaderTable()
{
	/** The shader-table layout is as follows:
		Entry 0 - Ray-gen program
		Entry 1 - Miss program
		Entry 2 - Hit program
		All entries in the shader-table must have the same size, so we will choose it base on the largest required entry.
		The ray-gen program requires the largest entry - sizeof(program identifier) + 8 bytes for a descriptor-table.
		The entry size must be aligned up to D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT
	*/

	// Calculate the size and create the buffer
	mShaderTableEntrySize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	mShaderTableEntrySize += 8; // The hit shader constant-buffer descriptor
	mShaderTableEntrySize = NV_ALIGN_TO(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT, mShaderTableEntrySize);
	const uint32_t shaderTableSize = mShaderTableEntrySize * 4;

	// For simplicity, we create the shader-table on the upload heap. You can also create it on the default heap
	mpShaderTable = DirectXUtil::AccelerationStructures::createBuffer(
		mpDevice,
		shaderTableSize,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		DirectXUtil::AccelerationStructures::kUploadHeapProps
	);

	// Map the buffer
	uint8_t* pData;
	NV_D3D_CALL(mpShaderTable->Map(0, nullptr, reinterpret_cast<void**>(&pData)));

	SampleFramework::ID3D12StateObjectPropertiesPtr pRtsoProps;
	mpPipelineState->QueryInterface(IID_PPV_ARGS(&pRtsoProps));

	// Entry 0 - ray-gen program ID and descriptor data
	memcpy(pData, pRtsoProps->GetShaderIdentifier(DirectXUtil::RTPipeline::kRayGenShader),
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	const uint64_t heapStart = mpSrvUavHeap->GetGPUDescriptorHandleForHeapStart().ptr;
	*reinterpret_cast<uint64_t*>(pData + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES) = heapStart;

	// Entry 1 - primary ray miss
	pData += mShaderTableEntrySize;
	memcpy(pData, pRtsoProps->GetShaderIdentifier(DirectXUtil::RTPipeline::kMissShader),
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

	// Entry 2 - miss program
	pData += mShaderTableEntrySize; // +1 skips ray-gen
	memcpy(pData, pRtsoProps->GetShaderIdentifier(DirectXUtil::RTPipeline::kShadowMiss),
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

	// Entry 3 - hit program
	pData += mShaderTableEntrySize;
	memcpy(pData, pRtsoProps->GetShaderIdentifier(DirectXUtil::RTPipeline::kHitGroup),
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	const uint64_t heapStartHit = mpSrvUavHeap->GetGPUDescriptorHandleForHeapStart().ptr;
	*reinterpret_cast<uint64_t*>(pData + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES) = heapStartHit;

	// Unmap
	mpShaderTable->Unmap(0, nullptr);
}

void AppWindow::createShaderResources()
{
	// Create the output resource. The dimensions and format should match the swap-chain
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// The backbuffer is actually DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, but sRGB formats can't be used with UAVs. We will convert to sRGB ourselves in the shader
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	resDesc.Width = mSwapChainSize.x;
	resDesc.Height = mSwapChainSize.y;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;

	NV_D3D_CALL(
		mpDevice->CreateCommittedResource(
			&DirectXUtil::AccelerationStructures::kDefaultHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			nullptr,
			IID_PPV_ARGS(&mpOutputResource)
		)
	);

	// Create an SRV/UAV/VertexSRV/IndexSRV descriptor heap. Need 4 entries - 1 SRV for the scene, 1 UAV for the output, 1 SRV for VertexBuffer, 1 SRV for IndexBuffer
	mpSrvUavHeap = D3D12UtilContext.createDescriptorHeap(mpDevice, 4, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);


	// Create the UAV. Based on the root signature we created it should be the first entry
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	mpDevice->CreateUnorderedAccessView(
		mpOutputResource.Get(),
		nullptr,
		&uavDesc,
		mpSrvUavHeap->GetCPUDescriptorHandleForHeapStart()
	);


	// Create the TLAS SRV right after the UAV. Note that we are using a different SRV desc here
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location = mTopLevelBuffers.pResult->GetGPUVirtualAddress();
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = mpSrvUavHeap->GetCPUDescriptorHandleForHeapStart();
	srvHandle.ptr += mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	mpDevice->CreateShaderResourceView(nullptr, &srvDesc, srvHandle);

	//Get the primitive buffers
	const DirectXUtil::AccelerationStructures::ShapeResources* primitiveRes =
		DirectXUtil::AccelerationStructures::createdPrimitive;

	// Index SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC indexSrvDesc = {};
	indexSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	indexSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	indexSrvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	indexSrvDesc.Buffer.NumElements = static_cast<int>(primitiveRes->indexCount * 2 / 4);
	indexSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
	indexSrvDesc.Buffer.StructureByteStride = 0;

	srvHandle.ptr += mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	indexSRVHandle = srvHandle;
	mpDevice->CreateShaderResourceView(primitiveRes->indexBuffer.Get(), &indexSrvDesc, indexSRVHandle);

	// Vertex SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC vertexSRVDesc = {};
	vertexSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	vertexSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	vertexSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexSRVDesc.Buffer.NumElements = static_cast<int>(primitiveRes->vertexCount);
	vertexSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	vertexSRVDesc.Buffer.StructureByteStride = sizeof(DirectXUtil::Structs::VertexPositionNormalTangentTexture);

	srvHandle.ptr += mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	vertexSRVHandle = srvHandle;
	mpDevice->CreateShaderResourceView(primitiveRes->vertexBuffer.Get(), &vertexSRVDesc, vertexSRVHandle);

	g_pd3dSrvDescHeapAlloc.Create(mpDevice.Get(), mpSrvUavHeap.Get());
}
