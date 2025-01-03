#include "EditorWindow.h"

#include "Logger.h"

namespace Anito
{
	void EditorWindow::onCreate()
	{
		Window::onCreate();
		Logger::log("Welcome to the Anito Tracer Editor!");
	}

	void EditorWindow::onUpdate()
	{
		Window::onUpdate();
	}

	void EditorWindow::onDestroy()
	{
		Window::onDestroy();
	}

	void EditorWindow::onFocus()
	{
		Window::onFocus();
	}

	void EditorWindow::onKillFocus()
	{
		Window::onKillFocus();
	}
}
