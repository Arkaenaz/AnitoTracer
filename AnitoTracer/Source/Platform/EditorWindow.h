#pragma once

#include "Window.h"

#include "Runtime/D3D12/D3D12SwapChain.h"

namespace Anito
{
	class EditorWindow : public Window
	{
	public:
		virtual void onCreate() override;
		virtual void onUpdate() override;
		virtual void onRender() override;
		virtual void onDestroy() override;
		virtual void onFocus() override;
		virtual void onKillFocus() override;

		void initializeEngine();
		void beginFrame(ID3D12GraphicsCommandList7* cmdList);
		void endFrame(ID3D12GraphicsCommandList7* cmdList);

	private:
		D3D12SwapChain* swapChain;
	};
}
