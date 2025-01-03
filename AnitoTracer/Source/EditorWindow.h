#pragma once

#include "Window.h"

#include "SwapChain.h"

namespace Anito
{
	class EditorWindow : public Window
	{
	public:
		virtual void onCreate() override;
		virtual void onUpdate() override;
		virtual void onDestroy() override;
		virtual void onFocus() override;
		virtual void onKillFocus() override;

		void initializeEngine();

	private:
		SwapChain* swapChain;
	};
}
