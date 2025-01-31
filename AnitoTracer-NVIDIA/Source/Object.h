#pragma once
#include <string>

#include "_externals/glm/glm/vec3.hpp"
#include "_externals/glm/glm/mat4x4.hpp"

// Constant buffer.
__declspec(align(16))
struct Constant {
	glm::mat4 world;
	glm::mat4 view;
	glm::mat4 proj;
};

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class Object
{
public:
	//typedef std::vector<Component*> ComponentList;
	enum ObjectType { primitive, mesh, camera };

	Object(std::string name, ObjectType type);
	~Object();

	virtual void Init();
	virtual void Update(float deltaTime);
	virtual void Draw();

	virtual void CalculateWorldMatrix();

	// Transform Functions
	virtual void Translate(glm::vec3 offset, float speed);
	virtual void RotateX(float radians);
	virtual void RotateY(float radians);
	virtual void RotateZ(float radians);
	virtual void Scale(glm::vec3 offset);

	// Getters/Setters
	virtual std::string GetName();
	virtual std::string GetTypeString();
	virtual bool GetActive();
	virtual glm::vec3 GetPosition();
	virtual glm::vec3 GetRotation();
	virtual glm::vec3 GetScale();
	virtual glm::mat4 GetWorldMat();

	/*Component* findComponentByName(String name);
	Component* findComponentByType(Component::ComponentType type, String name);
	ComponentList getAllComponents();
	ComponentList getComponentsOfType(Component::ComponentType type);*/

	virtual void SetName(std::string name);
	virtual void SetActive(bool active);
	virtual void SetPosition(glm::vec3 newPos);
	virtual void SetPosition(float x, float y, float z);
	virtual void SetRotation(glm::vec3 newRot);
	virtual void SetRotationX(float radians);
	virtual void SetRotationY(float radians);
	virtual void SetRotationZ(float radians);
	virtual void SetScale(glm::vec3 newScale);
	virtual void SetSelected(bool selected);

protected:
	std::string name;
	ObjectType type;
	bool isActive;
	bool isSelected;

	Constant mats;
	Transform transform;

	//ComponentList components;
};