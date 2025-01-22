#pragma once
#include <vector>

#include "GameObject.h"
#include "PrimitivesList.h"
#include "../Math/Math.h"

using namespace Math;

namespace Anito
{
	class GameObject;
	class GameObjectManager
	{
	public:
		void update(float deltaTime);
		void render();

		void addGameObject(ObjectType type, int count = 1);
		void removeGameObject(GameObject* go);

		void updateCameraView(Matrix4x4 worldCam);
		void setProjection(Matrix4x4 projMat);
		GameObject* findGameObject(std::string name);
		GameObject* getGameObject(int index);
		GameObject* getSelectedGameObject();
		std::vector<GameObject*> getGameObjects();
		//GameObject* checkCollision(Vector3 rayEndPoint);
		void setSelectedGameObject(GameObject* obj);
		void resetSelection();

	private:
		Quad* createQuad(ObjectType type, Vector2 size = Vector2(100, 100));
		/*Circle* createCircle(OBJECT_TYPE type);
		Cube* createCube(OBJECT_TYPE type);
		Plane* createPlane(OBJECT_TYPE type);
		Sphere* createSphere(OBJECT_TYPE type);
		Cylinder* createCylinder(OBJECT_TYPE type);
		Capsule* createCapsule(OBJECT_TYPE type);
		MeshObject* createMesh(OBJECT_TYPE type);*/

	private:
		std::vector<GameObject*> GOList;
		int quadCounter = 0;

	public:
		static GameObjectManager* getInstance();
		static bool initialize();
		static void destroy();

	private:
		static GameObjectManager* P_SHARED_INSTANCE;

		GameObjectManager();
		~GameObjectManager();
		GameObjectManager(GameObjectManager const&) {};
		GameObjectManager& operator = (GameObjectManager const&) {};
	};
}