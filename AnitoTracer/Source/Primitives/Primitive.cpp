#include "AnitoTracerPCH.h"
#include "Primitive.h"
//#include "InputSystem.h"

namespace Anito {
	Primitive::Primitive(std::string name, ObjectType type) :
		GameObject(name, type) {
	}

	Primitive::~Primitive() {}

	void Primitive::init()
	{
		this->initializeBuffers();
		this->createVertexShader();

		// Shader Attributes
		void* shaderByteCode = nullptr;
		size_t sizeShader = 0;
		std::vector<Vertex3D> vertices = this->createVertices();
		this->vb = D3D12RenderSystem::getInstance()->createVertexBuffer(vertices.data(), sizeof(Vertex3D), vertices.size());

		this->createPixelShader();
		this->createConstantBuffer();

		this->calculateBounds();
	}

	void Primitive::createVertexShader()
	{
		//RenderSystem* renderSystem = GraphicsEngine::get()->getRenderSystem();

		//// Shader Attributes
		//ShaderNames shaderNames;
		//void* shaderByteCode = nullptr;
		//size_t sizeShader = 0;

		//std::vector<Vertex3D> vertices = this->createVertices();

		//ShaderLibrary::getInstance()->requestVertexShaderData(shaderNames.BASE_VERTEX_SHADER_NAME, &shaderByteCode, &sizeShader);
		////renderSystem->compileVertexShader(L"TexturedVertexShader.hlsl", "vsmain", &shaderByteCode, &sizeShader);
		//this->vs = renderSystem->createVertexShader(shaderByteCode, (UINT)sizeShader);
		//this->vb = renderSystem->createVertexBuffer(vertices, sizeof(Vertex3D), shaderByteCode, sizeShader);
		//renderSystem->releaseCompiledShader();
	}

	void Primitive::createPixelShader()
	{
		//RenderSystem* renderSystem = GraphicsEngine::get()->getRenderSystem();

		//// Shader Attributes
		//void* shaderByteCode = nullptr;
		//size_t sizeShader = 0;

		//renderSystem->compilePixelShader(L"SolidPixelShader.hlsl", "psmain", &shaderByteCode, &sizeShader);
		//this->ps = renderSystem->createPixelShader(shaderByteCode, sizeShader);
		//renderSystem->releaseCompiledShader();
	}

	void Primitive::createConstantBuffer()
	{
		this->cb = D3D12RenderSystem::getInstance()->createConstantBuffer(&cc, sizeof(Constant));
	}

	void Primitive::update(float deltaTime)
	{
		GameObject::update(deltaTime);
	}

	void Primitive::draw()
	{
		//GraphicsEngine::get()->getRenderSystem()->setRasterizerState(false);

		D3D12CommandContext* command = D3D12RenderSystem::getInstance()->getDXContext();

		//ShaderNames shaderNames;
		//ShaderLibrary* shaderLib = ShaderLibrary::getInstance();
		//command->setRenderConfig(shaderLib->getVertexShader(shaderNames.BASE_VERTEX_SHADER_NAME), shaderLib->getPixelShader(shaderNames.BASE_PIXEL_SHADER_NAME));
		//command->resetTexture();

		// Bind to Shaders.
		/*command->setConstantBuffer(vs, this->cb);
		command->setConstantBuffer(ps, this->cb);*/

		//// Set Shaders.
		//device->setVertexShader(vs);
		//device->setPixelShader(ps);

		// Draw Object.
		/*command->setVertexBuffer(this->vb);
		command->setIndexBuffer(this->ib);
		command->drawIndexedTriangleList(this->ib->getSizeIndexList(), 0, 0);*/
	}
}