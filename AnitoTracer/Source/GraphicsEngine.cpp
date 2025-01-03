#include "GraphicsEngine.h"

#include "Logger.h"

namespace Anito
{
	RenderSystem* GraphicsEngine::getRenderSystem()
	{
		return this->renderSystem;
	}

	GraphicsEngine* GraphicsEngine::getInstance()
	{
		return P_SHARED_INSTANCE;
	}
	void GraphicsEngine::initialize()
	{
		if (P_SHARED_INSTANCE)
		{
			Logger::debug("Graphics Engine already created");
		}
		P_SHARED_INSTANCE = new GraphicsEngine();
	}
	void GraphicsEngine::destroy()
	{
		delete P_SHARED_INSTANCE;
	}
	GraphicsEngine* GraphicsEngine::P_SHARED_INSTANCE = nullptr;
	GraphicsEngine::GraphicsEngine()
	{
		this->renderSystem = new RenderSystem();
		Logger::debug(this, "Initialized");
	}
	GraphicsEngine::~GraphicsEngine()
	{
		delete renderSystem;
		P_SHARED_INSTANCE = nullptr;
		Logger::debug(this, "Destroyed");
	}
	GraphicsEngine::GraphicsEngine(const GraphicsEngine&) {}
}
