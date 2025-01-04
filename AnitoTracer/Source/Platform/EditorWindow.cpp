#include "AnitoTracerPCH.h"
#include "EditorWindow.h"

#include "Runtime/D3D12/D3D12DebugLayer.h"

#include "Runtime/D3D12/D3D12RenderSystem.h"
#include "Runtime/D3D12/D3D12DeviceContext.h"

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
		auto* cmdList = renderSystem->getDXContext()->initCommandList(this->swapChain->getFrameIndex());

		// Begin Frame
		this->beginFrame(cmdList);

		// Populate command list
		renderSystem->getDXContext()->setViewportSize(width, height);
		renderSystem->getDXContext()->clearRenderTargetColor(this->swapChain, 0.207, 0.145, 0.223, 1);

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

		delete this->swapChain;
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

		// Initialize the Swap Chain
		RECT windowRect = this->getClientWindowRect();

		FLOAT width = windowRect.right - windowRect.left;
		FLOAT height = windowRect.bottom - windowRect.top;

		this->swapChain = renderSystem->createSwapChain(this->windowHandle, width, height);

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
