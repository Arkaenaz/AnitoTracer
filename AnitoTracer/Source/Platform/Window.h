#pragma once

namespace Anito
{
	class Window
	{
	public:
		Window();
		virtual ~Window();

		virtual void onCreate() = 0;
		virtual void onUpdate() = 0;
		virtual void onRender() = 0;
		virtual void onDestroy() = 0;
		virtual void onResize(UINT width, UINT height) = 0;
		virtual void onFocus();
		virtual void onKillFocus();

		bool run();

		RECT getClientWindowRect();

	protected:
		HWND windowHandle;
		bool isRunning = false;
		bool isInitialized = false;
	};
}