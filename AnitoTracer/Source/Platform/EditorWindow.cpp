#include "EditorWindow.h"

#include "Runtime/D3D12/D3D12DebugLayer.h"

#include "Runtime/D3D12/D3D12RenderSystem.h"
#include "Runtime/D3D12/D3D12DeviceContext.h"

#include "Common/Logger.h"

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
		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		auto* cmdList = renderSystem->getImmediateDeviceContext()->initCommandList();

		renderSystem->getImmediateDeviceContext()->clearRenderTargetColor(this->swapChain, 1, 0, 0, 1);

		renderSystem->getImmediateDeviceContext()->executeCommandList();
		this->swapChain->present(false);
	}

	void EditorWindow::onDestroy()
	{
		Window::onDestroy();
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
}
