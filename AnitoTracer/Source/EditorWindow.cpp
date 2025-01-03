#include "EditorWindow.h"

#include "DebugLayer.h"
#include "GraphicsEngine.h"

#include "Logger.h"

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
		RenderSystem* renderSystem = GraphicsEngine::getInstance()->getRenderSystem();

		auto* cmdList = renderSystem->getImmediateDeviceContext()->initCommandList();

		renderSystem->getImmediateDeviceContext()->executeCommandList();
		this->swapChain->present(false);
	}

	void EditorWindow::onDestroy()
	{
		Window::onDestroy();
		GraphicsEngine::destroy();
		DebugLayer::destroy();
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
		DebugLayer::initialize();
		GraphicsEngine::initialize();

		RenderSystem* renderSystem = GraphicsEngine::getInstance()->getRenderSystem();

		// Initialize the Swap Chain
		RECT windowRect = this->getClientWindowRect();

		FLOAT width = windowRect.right - windowRect.left;
		FLOAT height = windowRect.bottom - windowRect.top;

		this->swapChain = renderSystem->createSwapChain(this->windowHandle, width, height);

		Logger::debug(this, "Initialized Engine");
	}
}
