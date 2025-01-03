#pragma once

#include "RenderSystem.h"

namespace Anito
{
	class GraphicsEngine
	{
	public:
		RenderSystem* getRenderSystem();

	private:
		RenderSystem* renderSystem = nullptr;

	public:
		static GraphicsEngine* getInstance();
		static void initialize();
		static void destroy();

	private:
		static GraphicsEngine* P_SHARED_INSTANCE;

		GraphicsEngine();
		~GraphicsEngine();
		GraphicsEngine(GraphicsEngine const&);
		GraphicsEngine& operator = (GraphicsEngine const&);
	};
}