#include "AnitoTracerPCH.h"
#include "EditorWindow.h"

#include "Runtime/D3D12/D3D12DebugLayer.h"

#include "Runtime/D3D12/D3D12RenderSystem.h"
#include "Runtime/D3D12/D3D12CommandContext.h"

namespace Anito
{
	void EditorWindow::onCreate()
	{
		Window::onCreate();
		this->initializeEngine();
		Logger::log("\nWelcome to the Anito Tracer Editor!\n");
	}

	void EditorWindow::onUpdate()
	{
		Window::onUpdate();
	}

	void EditorWindow::onRender()
	{
		Window::onRender();
		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		RECT windowRect = this->getClientWindowRect();

		const FLOAT width = static_cast<FLOAT>(windowRect.right - windowRect.left);
		const FLOAT height = static_cast<FLOAT>(windowRect.bottom - windowRect.top);

		// Resets and Reinitializes Command List
		auto* cmdList = renderSystem->getDXContext()->initCommandList(this->swapChain->getFrameIndex(), this->pipelineState->getDXState());

		cmdList->SetGraphicsRootSignature(this->rootSignature);
		renderSystem->getDXContext()->setViewportSize(width, height);

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
		Window::onResize(width, height);
		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		this->swapChain->cleanRenderTarget();
		this->swapChain->resizeBuffers(width, height);
		this->swapChain->createRenderTarget();
		renderSystem->getDXContext()->moveToNextFrame(this->swapChain);
	}

	void EditorWindow::onDestroy()
	{
		Window::onDestroy();
		D3D12RenderSystem::getInstance()->getDXContext()->signalAndWaitForGpu(this->swapChain->getFrameIndex());
		
		delete this->vertexBuffer;
		delete this->swapChain;
		delete this->pipelineState;
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

		this->graphicsPipeline = D3D12GraphicsPipeline();

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
		Logger::log(fullPath);
		D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
		D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

		D3D12_SHADER_BYTECODE vertexShaderByteCode = CD3DX12_SHADER_BYTECODE(vertexShader);
		D3D12_SHADER_BYTECODE pixelShaderByteCode = CD3DX12_SHADER_BYTECODE(pixelShader);

		this->graphicsPipeline.setRootSignature(this->rootSignature);
		this->graphicsPipeline.setVertexShader(vertexShaderByteCode);
		this->graphicsPipeline.setPixelShader(pixelShaderByteCode);

		// Initialize Pipeline State
		this->pipelineState = renderSystem->createPipelineState(this->graphicsPipeline);

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
		auto* cmdList = renderSystem->getDXContext()->initCommandList(this->swapChain->getFrameIndex(), this->pipelineState->getDXState());

		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * (width / height), 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
			{ { 0.25f, -0.25f * (width / height), 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f * (width / height), 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		// Create Vertex Buffer
		this->vertexBuffer = renderSystem->createVertexBuffer(triangleVertices, sizeof(Vertex), ARRAYSIZE(triangleVertices));

		// Close the command list
		cmdList->Close();

		Logger::debug(this, "Initialized Engine");
	}
}
