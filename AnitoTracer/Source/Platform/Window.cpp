#include "AnitoTracerPCH.h"
#include "Window.h"

LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	using namespace Anito;
	switch (msg)
	{
	case WM_SIZE:
		{
		if (wParam == SIZE_MINIMIZED)
		{
			return 0;
		}
		break;
		}
	case WM_CREATE:
	{
		// Event fired when the window is created
		// collected here...

		break;
	}
	case WM_SETFOCUS:
	{
		// Event fired when the window gets focus
		Window* window = (Window*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);
		if (window)
		{
			window->onFocus();
		}
		break;
	}
	case WM_KILLFOCUS:
	{
		// Event fired when the window loses focus
		Window* window = (Window*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);
			if (window)
			{
				window->onKillFocus();
			}
		break;
	}
	case WM_DESTROY:
	{
		// Event fired when the window is destroyed
		Window* window = (Window*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);
		window->onDestroy();
		::PostQuitMessage(0);
		break;
	}
	case WM_DPICHANGED:
	{
		break;
	}
	default:
		return ::DefWindowProc(windowHandle, msg, wParam, lParam);
	}
}

namespace Anito {
	Window::Window()
	{
		WNDCLASSEX wc;
		wc.cbClsExtra = NULL;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.cbWndExtra = NULL;
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wc.hInstance = NULL;
		wc.lpszClassName = L"MyWindowClass";
		wc.lpszMenuName = L"";
		wc.style = NULL;
		wc.lpfnWndProc = &WndProc;

		if (!::RegisterClassEx(&wc))
		{
			Logger::error(this, "Window not created successfully");
		}

		this->windowHandle = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyWindowClass",
			L"AnitoTracer", WS_OVERLAPPEDWINDOW, 0, 0, 1440, 900,
			NULL, NULL, NULL, NULL);

		if (!this->windowHandle)
		{
			Logger::error(this, "Window not created successfully");
		}

		::ShowWindow(this->windowHandle, SW_SHOW);
		::UpdateWindow(this->windowHandle);

		this->isRunning = true;
	}

	Window::~Window()
	{
	}

	bool Window::broadcast()
	{
		if (!this->isInitialized)
		{
			SetWindowLongPtr(this->windowHandle, GWLP_USERDATA, (LONG_PTR)this);
			this->onCreate();
			this->isInitialized = true;
		}

		this->onUpdate();

		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Sleep(1);

		return true;
	}

	bool Window::running()
	{
		return this->isRunning;
	}

	RECT Window::getClientWindowRect()
	{
		RECT rect;
		::GetClientRect(this->windowHandle, &rect);
		::ClientToScreen(this->windowHandle, (POINT*)&rect);

		return rect;
	}

	void Window::onCreate()
	{
	}

	void Window::onUpdate()
	{

	}

	void Window::onDestroy()
	{
		this->isRunning = false;
	}

	void Window::onFocus()
	{
	}

	void Window::onKillFocus()
	{
	}
}