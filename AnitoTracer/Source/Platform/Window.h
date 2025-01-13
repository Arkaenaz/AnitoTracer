#pragma once

namespace Anito
{
	class Window
	{
	public:
		Window();
		virtual ~Window();

		virtual void onCreate();
		virtual void onUpdate();
		virtual void onRender();
		virtual void onDestroy();
		virtual void onResize(UINT width, UINT height);
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