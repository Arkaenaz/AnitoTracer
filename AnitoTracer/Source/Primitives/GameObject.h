#pragma once
#include <iostream>
#include <string>

#include "../Runtime/D3D12/D3D12RenderSystem.h"
#include "../Runtime/D3D12/D3D12ConstantBuffer.h"

#include "../Common/Structs.h"
#include "../Math/Math.h"

using namespace Math;

namespace Anito
{
	enum class ObjectType {
		// Primitives
		quad = 0, circle, cube, plane, sphere, cylinder, capsule,
		// Camera
		scene_camera, ortho_camera, perspective_camera,
	};

	struct Transform
	{
		Vector3 position, scale, rotation;
	};

	class GameObjectManager;
	class GameObject
	{
	public:
		typedef std::string String;
		//typedef std::vector<Component*> ComponentList;

	protected:
		D3D12ConstantBuffer* cb;

		std::string name;
		bool isActive;
		ObjectType type;

		Constant cc;
		Vector3 size;

		bool isSelected;
		Transform transform;

		float deltaTime;

	public:
		bool physOn;

	private:
		friend class GameObjectManager;

	public:
		GameObject(std::string name, ObjectType type);
		~GameObject();
		virtual void calculateBounds();
		virtual void calculateWorldMatrix();

		virtual void awake();
		virtual void update(float deltaTime);
		virtual void draw();

		// Editing Mode
		virtual void edit(float deltaTime);

		// Transform Functions
		/*virtual void translate(Math::Vector3 offset, float speed);
		virtual void rotateX(float radians);
		virtual void rotateY(float radians);
		virtual void rotateZ(float radians);
		virtual void scale(Math::Vector3 offset);*/

		// Entity Component System
		/*void attachComponent(Component* comp);
		void detachComponent(Component* comp);*/

		// Getters/Setters
		virtual std::string getName();
		virtual std::string getTypeString();
		virtual bool getActive();
		virtual Vector3 getLocalPosition();
		virtual Vector3 getPosition();
		virtual Vector3 getLocalRotation();
		virtual Vector3 getRotation();
		virtual Vector3 getScale();
		/*virtual bool isWithinBounds(Math::Vector3 ray);
		float* getPhysicsLocalMatrix();*/
		virtual Matrix4x4 getWorldMat();

		/*Component* findComponentByName(String name);
		Component* findComponentByType(Component::ComponentType type, String name);
		ComponentList getAllComponents();
		ComponentList getComponentsOfType(Component::ComponentType type);*/

		virtual void setName(std::string name);
		virtual void setActive(bool active);
		/*virtual void setWorldMat(float matrix[16]);*/
		virtual void setPosition(Math::Vector3 newPos);
		virtual void setPosition(float x, float y, float z);
		virtual void setRotation(Math::Vector3 newRot);
		virtual void setRotation(Math::Vector4 newRot);
		virtual void setRotationX(float xOffset);
		virtual void setRotationY(float yOffset);
		virtual void setRotationZ(float zOffset);
		virtual void setScale(Math::Vector3 newScale);
		virtual void setSelected(bool selected);
	};

}