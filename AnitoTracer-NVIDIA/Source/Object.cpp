#include "Object.h"

#include "CameraManager.h"
#include "Math.h"

Object::Object(std::string name, ObjectType type)
	: name(name), type(type)
{
	this->isActive = true;
	this->isSelected = false;
	this->transform.position = glm::vec3(0);
	this->transform.rotation = glm::vec3(0);
	this->transform.scale = glm::vec3(1);

	this->mats.world = glm::mat4(0.f);
	this->mats.view = glm::mat4(1.f);
	this->mats.proj = glm::mat4(1.f);
}

Object::~Object() {}

void Object::Init()
{
	// TODO: Constant Buffer Creation
}

void Object::Update(float deltaTime)
{
	this->CalculateWorldMatrix();

	this->mats.view = CameraManager::getInstance()->GetActiveCamera()->GetViewMatrix();
	this->mats.proj = CameraManager::getInstance()->GetActiveCamera()->GetProjMatrix();

	// TODO: Update the constant buffer.
	//this->cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &this->mats);
}

void Object::Draw()
{
}

void Object::CalculateWorldMatrix()
{
	this->mats.world = glm::mat4(1.f);

	// Scale
	glm::mat4 scale = glm::mat4(1.0f);
	scale = SetToMatScale(this->transform.scale);

	// Rotation
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 temp = glm::mat4(1.0f);

	temp = SetToMatRotationX(this->transform.rotation.x);
	rotation *= temp;

	temp = glm::mat4(1.0f);
	temp = SetToMatRotationY(this->transform.rotation.y);
	rotation *= temp;

	temp = glm::mat4(1.0f);
	temp = SetToMatRotationZ(this->transform.rotation.z);
	rotation *= temp;

	// Translate
	glm::mat4 translate = glm::mat4(1.0f);
	translate = SetToMatTranslation(transform.position);

	//std::cout << name << " calcWorldMatrix(): Scaling." << std::endl;
	this->mats.world *= scale;
	//std::cout << name << " calcWorldMatrix(): Rotating." << std::endl;
	this->mats.world *= rotation;
	//std::cout << name << " calcWorldMatrix(): Translating." << std::endl;
	this->mats.world *= translate;
}

void Object::Translate(glm::vec3 offset, float speed)
{
	this->transform.position += offset * speed /*(float)EngineTime::getDeltaTime()*/;

	this->CalculateWorldMatrix();
}

void Object::RotateX(float radians)
{
	this->transform.rotation.x += radians;
	this->CalculateWorldMatrix();
}

void Object::RotateY(float radians)
{
	this->transform.rotation.y += radians;
	this->CalculateWorldMatrix();
}

void Object::RotateZ(float radians)
{
	this->transform.rotation.z += radians;
	this->CalculateWorldMatrix();
}

void Object::Scale(glm::vec3 offset)
{
	this->transform.scale += offset /*(float)EngineTime::getDeltaTime()*/;
	this->CalculateWorldMatrix();
}

std::string Object::GetName()
{
	return this->name;
}

std::string Object::GetTypeString()
{
	switch (this->type)
	{
	case primitive:
		return "Primitive";
	case mesh:
		return "Mesh";
	case camera:
		return "Camera";
	default:
		return "Unknown";
	}
}

bool Object::GetActive()
{
	return this->isActive;
}

glm::vec3 Object::GetPosition()
{
	return this->transform.position;
}

glm::vec3 Object::GetRotation()
{
	return this->transform.rotation;
}

glm::vec3 Object::GetScale()
{
	return this->transform.scale;
}

glm::mat4 Object::GetWorldMat()
{
	return this->mats.world;
}

void Object::SetName(std::string name)
{
	this->name = name;
}

void Object::SetActive(bool active)
{
	this->isActive = active;
}

void Object::SetPosition(glm::vec3 newPos)
{
	this->transform.position = newPos;
}

void Object::SetPosition(float x, float y, float z)
{
	this->transform.position = glm::vec3(x, y, z);
	this->CalculateWorldMatrix();
}

void Object::SetRotation(glm::vec3 newRot)
{
	this->transform.rotation.x = newRot.x;
	this->transform.rotation.y = newRot.y;
	this->transform.rotation.z = newRot.z;
	this->CalculateWorldMatrix();
}

void Object::SetRotationX(float radians)
{
	this->transform.rotation.x = radians;
	this->CalculateWorldMatrix();
}

void Object::SetRotationY(float radians)
{
	this->transform.rotation.y = radians;
	this->CalculateWorldMatrix();
}

void Object::SetRotationZ(float radians)
{
	this->transform.rotation.z = radians;
	this->CalculateWorldMatrix();
}

void Object::SetScale(glm::vec3 newScale)
{
	this->transform.scale = newScale;
	this->CalculateWorldMatrix();
}

void Object::SetSelected(bool selected)
{
	this->isSelected = selected;
}
