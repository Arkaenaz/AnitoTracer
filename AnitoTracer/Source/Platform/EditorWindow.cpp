#include "AnitoTracerPCH.h"
#include "EditorWindow.h"

#include "Runtime/D3D12/D3D12DebugLayer.h"

#include "Runtime/D3D12/D3D12RenderSystem.h"
#include "Runtime/D3D12/D3D12CommandContext.h"
#include "Runtime/D3D12/Raytracing/AccelerationStructures.h"

namespace Anito
{
	const wchar_t* EditorWindow::c_hitGroupName = L"MyHitGroup";
	const wchar_t* EditorWindow::c_raygenShaderName = L"MyRaygenShader";
	const wchar_t* EditorWindow::c_closestHitShaderName = L"MyClosestHitShader";
	const wchar_t* EditorWindow::c_missShaderName = L"MyMissShader";

	void EditorWindow::onCreate()
	{
		this->initializeEngine();
		Logger::log("\nWelcome to the Anito Tracer Editor!\n");
	}

	void EditorWindow::onUpdate()
	{

	}

	void EditorWindow::onRender()
	{
		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		RECT windowRect = this->getClientWindowRect();

		const UINT width = windowRect.right - windowRect.left;
		const UINT height = windowRect.bottom - windowRect.top;

		// Resets and Reinitializes Command List
		D3D12CommandContext* commandContext = renderSystem->getDXContext();

		auto* cmdList = commandContext->initCommandList(this->swapChain->getFrameIndex(), this->graphicsPipeline->getDXState());

		cmdList->SetGraphicsRootSignature(this->rootSignature);

		renderSystem->getDXContext()->setViewportandScissor(windowRect.left, windowRect.top, width, height);

		// Begin Frame
		renderSystem->getDXContext()->beginFrame(this->swapChain->getCurrentRenderTarget());

		// Populate command list
		renderSystem->getDXContext()->clearRenderTargetColor(this->swapChain, 0.207f, 0.145f, 0.223f, 1);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = this->vertexBuffer->getVertexBufferView();
		cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
		cmdList->DrawInstanced(3, 1, 0, 0);

		// End Frame
		renderSystem->getDXContext()->endFrame(this->swapChain->getCurrentRenderTarget());

		// Executes command list
		renderSystem->getDXContext()->executeCommandList();

		this->swapChain->present(false);

		renderSystem->getDXContext()->moveToNextFrame(this->swapChain);
	}

	void EditorWindow::onResize(UINT width, UINT height)
	{
		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		this->swapChain->cleanRenderTarget();
		renderSystem->getDXContext()->resetFenceValues();
		this->swapChain->resizeBuffers(width, height);
		this->swapChain->createRenderTarget();
		renderSystem->getDXContext()->moveToNextFrame(this->swapChain);
	}

	void EditorWindow::onDestroy()
	{
		D3D12RenderSystem::getInstance()->getDXContext()->signalAndWaitForGpu(this->swapChain->getFrameIndex());
		this->swapChain->updateFrameIndex();
		D3D12RenderSystem::getInstance()->getDXContext()->signalAndWaitForGpu(this->swapChain->getFrameIndex());

		delete this->vertexBuffer;
		delete this->swapChain;
		delete this->graphicsPipeline;
		this->rootSignature->Release();
		D3D12RenderSystem::destroy();
		D3D12DebugLayer::destroy();
	}

	void EditorWindow::onFocus()
	{
		Window::onFocus();
	}

	void EditorWindow::onKillFocus()
	{
		Window::onKillFocus();
	}

	void EditorWindow::initializeEngine()
	{
		D3D12DebugLayer::initialize();
		D3D12RenderSystem::initialize();

		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		this->graphicsPipeline = new D3D12GraphicsPipelineObject(*renderSystem->getDevice());

		ID3D10Blob* signature;
		ID3D10Blob* error;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		renderSystem->getDevice()->get()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->rootSignature));

		if (signature)
			signature->Release();
		if (error)
			error->Release();

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

		this->graphicsPipeline->setRootSignature(this->rootSignature);
		this->graphicsPipeline->setVertexShader(CD3DX12_SHADER_BYTECODE(vertexShader));
		this->graphicsPipeline->setPixelShader(CD3DX12_SHADER_BYTECODE(pixelShader));
		this->graphicsPipeline->create();

		if (vertexShader)
			vertexShader->Release();
		if (pixelShader)
			pixelShader->Release();

		// Initialize the Swap Chain
		RECT windowRect = this->getClientWindowRect();

		UINT width = static_cast<UINT>(windowRect.right - windowRect.left);
		UINT height = static_cast<UINT>(windowRect.bottom - windowRect.top);

		this->swapChain = renderSystem->createSwapChain(this->windowHandle, width, height);

		// Start up the command list for other initialization
		auto* cmdList = renderSystem->getDXContext()->initCommandList(this->swapChain->getFrameIndex(), this->graphicsPipeline->getDXState());

		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * (width / height), 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
			{ { 0.25f, -0.25f * (width / height), 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f * (width / height), 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		// Create Vertex Buffer
		this->vertexBuffer = renderSystem->createVertexBuffer(triangleVertices, sizeof(Vertex), ARRAYSIZE(triangleVertices));

		this->CreateRootSignatures();
		this->createAccelerationStructures(cmdList);
		// Close the command list
		cmdList->Close();

		Logger::debug(this, "Initialized Engine");
	}

	void EditorWindow::CreateRootSignatures()
	{
		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();
		// Global Root Signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
		{
			CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
			UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
			CD3DX12_ROOT_PARAMETER rootParameters[2];
			rootParameters[0].InitAsDescriptorTable(1, &UAVDescriptor);
			rootParameters[1].InitAsShaderResourceView(0);
			CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);

			ID3D10Blob* signature;
			ID3D10Blob* error;

			D3D12SerializeRootSignature(&globalRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			renderSystem->getDevice()->get()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->m_raytracingGlobalRootSignature));

			if (signature)
				signature->Release();
			if (error)
				error->Release();
		}

		// Local Root Signature
		// This is a root signature that enables a shader to have unique arguments that come from shader tables.
		{
			CD3DX12_ROOT_PARAMETER rootParameters[1];
			rootParameters[0].InitAsConstants(sizeof(RayGenConstantBuffer), 0, 0);
			CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
			localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

			ID3D10Blob* signature;
			ID3D10Blob* error;

			D3D12SerializeRootSignature(&localRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			renderSystem->getDevice()->get()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->m_raytracingLocalRootSignature));

			if (signature)
				signature->Release();
			if (error)
				error->Release();
		}
	}

	/*void EditorWindow::CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
	{
		// Hit group and miss shaders in this sample are not using a local root signature and thus one is not associated with them.

		// Local root signature to be used in a ray gen shader.
		{
			auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			localRootSignature->SetRootSignature(m_raytracingLocalRootSignature);
			// Shader association
			auto rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
			rootSignatureAssociation->AddExport(c_raygenShaderName);
		}
	}

	void EditorWindow::CreateRaytracingPipelineStateObject()
	{
		// Create 7 subobjects that combine into a RTPSO:
	// Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
	// Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
	// This simple sample utilizes default shader association except for local root signature subobject
	// which has an explicit association specified purely for demonstration purposes.
	// 1 - DXIL library
	// 1 - Triangle hit group
	// 1 - Shader config
	// 2 - Local root signature and association
	// 1 - Global root signature
	// 1 - Pipeline config
		CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };


		// DXIL library
		// This contains the shaders and their entrypoints for the state object.
		// Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
		auto lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		//D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void*)g_pRaytracing, ARRAYSIZE(g_pRaytracing));
		//lib->SetDXILLibrary(&libdxil);
		// Define which shader exports to surface from the library.
		// If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
		// In this sample, this could be omitted for convenience since the sample uses all shaders in the library. 
		{
			lib->DefineExport(c_raygenShaderName);
			lib->DefineExport(c_closestHitShaderName);
			lib->DefineExport(c_missShaderName);
		}

		// Triangle hit group
		// A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
		// In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
		auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
		hitGroup->SetHitGroupExport(c_hitGroupName);
		hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		// Shader config
		// Defines the maximum sizes in bytes for the ray payload and attribute structure.
		auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		UINT payloadSize = 4 * sizeof(float);   // float4 color
		UINT attributeSize = 2 * sizeof(float); // float2 barycentrics
		shaderConfig->Config(payloadSize, attributeSize);

		// Local root signature and shader association
		CreateLocalRootSignatureSubobjects(&raytracingPipeline);
		// This is a root signature that enables a shader to have unique arguments that come from shader tables.

		// Global root signature
		// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
		auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
		globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature);

		// Pipeline config
		// Defines the maximum TraceRay() recursion depth.
		auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		// PERFOMANCE TIP: Set max recursion depth as low as needed 
		// as drivers may apply optimization strategies for low recursion depths. 
		UINT maxRecursionDepth = 1; // ~ primary rays only. 
		pipelineConfig->Config(maxRecursionDepth);

#if _DEBUG
		PrintStateObjectDesc(raytracingPipeline);
#endif

		// Create the state object.
		D3D12RenderSystem::getInstance()->getDevice()->get()->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject));
	}*/

	void EditorWindow::createAccelerationStructures(ID3D12GraphicsCommandList10* cmdList)
	{
		AccelerationStructures acs;

		long mTlasSize = 0;

		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		AccelerationStructureBuffers bottomLevelBuffers = acs.CreateBottomLevelAS(*renderSystem->getDevice()->get(), cmdList, this->vertexBuffer);
		AccelerationStructureBuffers topLevelBuffers = acs.CreateTopLevelAS(*renderSystem->getDevice()->get(), cmdList, bottomLevelBuffers.accelerationStructure->get(), mTlasSize);

		// The tutorial doesn't have any resource lifetime management, so we flush and sync here. This is not required by the DXR spec - you can submit the list whenever you like as long as you take care of the resources lifetime.
		/*mFenceValue = context.SubmitCommandList(mpCmdList, mpCmdQueue, mpFence, mFenceValue);
		mpFence.SetEventOnCompletion(mFenceValue, mFenceEvent);
		mFenceEvent.WaitOne();
		int bufferIndex = mpSwapChain.GetCurrentBackBufferIndex();
		mpCmdList.Reset(mFrameObjects[0].pCmdAllocator, null);*/

		topLevelAS = topLevelBuffers.accelerationStructure;
		bottomLevelAS = bottomLevelBuffers.accelerationStructure;
	}
}
