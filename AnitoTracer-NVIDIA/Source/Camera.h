#pragma once

#include "Object.h" 
#include "_externals/glm/glm/ext/matrix_transform.hpp"

const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 1000.0f;

class Camera : public Object
{
public:
	enum CameraType { orthographic, perspective, topdown, sidescroller };

	Camera(std::string name, CameraType camType = orthographic, ObjectType type = camera);
	~Camera();

	virtual void Update(float deltaTime);
	virtual void CheckForInput();
	virtual void UpdateViewMatrix();
	virtual void UpdateProjectionMatrix();
	virtual void Move(glm::vec3 offset);

	float GetPanSpeed();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjMatrix();
	glm::vec3 GetCameraCenter();
	void SetCenterOffset(glm::vec3 offset);
	void SetForward(float dir);
	void SetRightward(float dir);
	void SetProjectionType(int type);

protected:
	CameraType camType;
	glm::mat4 prevCamMat;
	glm::vec3 worldUp; // World Up Direction
	glm::vec3 center; // Camera Center

	int type = 0;
	float forward = 0;
	float rightward = 0;
	float panSpeed = 0.1f;
	float moveSpeed = 1.0f;

	bool moving = false;
	bool updatedCameraProjPos = false;

	//GameObject* lastSelectedGO;
	bool firstMouseDown = true;
	bool leftMouseDown = false;
	glm::vec2 lastHitPos;
	glm::vec2 deltaHitPos;

	bool firstRecord = true;

	glm::vec2 oldMousePos;
};

