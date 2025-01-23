#include "AnitoTracerPCH.h"
#include "Quad.h"

namespace Anito
{
	Quad::Quad(std::string name, Vector2 size) : GameObject(name, ObjectType::quad)
	{
		this->width = size.x;
		this->height = size.y;

		D3D12RenderSystem* renderSystem = D3D12RenderSystem::getInstance();

		// Initializing the vertices of the Triangle.
		Vertex vertices[] =
		{
			{ { 0.0f, 0.5f * (width / height), 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
			{ { 0.5f, -0.5f * (width / height), 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.5f, -0.5f * (width / height), 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		UINT size_list = ARRAYSIZE(vertices);
		this->vb = renderSystem->createVertexBuffer(vertices, sizeof(Vertex), ARRAYSIZE(vertices));
	}

	Quad::~Quad() {}

	void Quad::update(float deltaTime)
	{
		GameObject::update(deltaTime);
	}

	void Quad::draw()
	{
		ID3D12GraphicsCommandList10* cmdList = D3D12RenderSystem::getInstance()->getDXContext()->getCommandList();

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = this->vb->getVertexBufferView();
		cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
		cmdList->DrawInstanced(3, 1, 0, 0);
	}
}
