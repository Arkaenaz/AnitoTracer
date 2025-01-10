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
		Window::onUpdate();
		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		RECT windowRect = this->getClientWindowRect();

		FLOAT width = windowRect.right - windowRect.left;
		FLOAT height = windowRect.bottom - windowRect.top;

		// Resets and Reinitializes Command List
		auto* cmdList = renderSystem->getDXContext()->initCommandList(this->swapChain->getFrameIndex(), this->pipelineState->getDXState());

		cmdList->SetGraphicsRootSignature(this->pipelineState->getRootSignature());
		renderSystem->getDXContext()->setViewportSize(width, height);

		// Begin Frame
		this->beginFrame(cmdList);

		// Populate command list
		renderSystem->getDXContext()->clearRenderTargetColor(this->swapChain, 0.207, 0.145, 0.223, 1);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = this->vertexBuffer->getVertexBufferView();
		cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
		cmdList->DrawInstanced(3, 1, 0, 0);

		// End Frame
		this->endFrame(cmdList);

		// Executes command list
		renderSystem->getDXContext()->executeCommandList();

		this->swapChain->present(false);

		renderSystem->getDXContext()->moveToNextFrame(this->swapChain);
	}

	void EditorWindow::onDestroy()
	{
		Window::onDestroy();

		delete this->vertexBuffer;
		delete this->swapChain;
		delete this->pipelineState;
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

		// Initialize Pipeline State
		this->pipelineState = renderSystem->createPipelineState();

		// Initialize the Swap Chain
		RECT windowRect = this->getClientWindowRect();

		FLOAT width = windowRect.right - windowRect.left;
		FLOAT height = windowRect.bottom - windowRect.top;

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

	void EditorWindow::beginFrame(ID3D12GraphicsCommandList7* cmdList)
	{
		UINT frameIndex = this->swapChain->getFrameIndex();

		// Indicate that the back buffer will be used as a render target.
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(this->swapChain->getRenderTarget(frameIndex), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmdList->ResourceBarrier(1, &barrier);
	}

	void EditorWindow::endFrame(ID3D12GraphicsCommandList7* cmdList)
	{
		UINT frameIndex = this->swapChain->getFrameIndex();

		// Indicate that the back buffer will now be used to present.
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(this->swapChain->getRenderTarget(frameIndex), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		cmdList->ResourceBarrier(1, &barrier);
	}
}
