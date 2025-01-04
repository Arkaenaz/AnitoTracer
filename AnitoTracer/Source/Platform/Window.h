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
		virtual void onFocus();
		virtual void onKillFocus();

		bool broadcast();
		bool running();

		RECT getClientWindowRect();

	protected:
		HWND windowHandle;
		bool isRunning = false;
		bool isInitialized = false;
	};
}