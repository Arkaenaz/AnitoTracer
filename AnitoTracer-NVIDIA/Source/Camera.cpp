#include "Camera.h"

#include "Math.h"
#include <_externals/glm/glm/ext/matrix_clip_space.hpp>

Camera::Camera(std::string name, CameraType camType, ObjectType type) :
	Object(name, type)
{
	this->camType = camType;

	this->transform.position = glm::vec3(0, 3, -50);
	this->worldUp = glm::vec3(0, 1, 0);
	this->center = glm::normalize(glm::vec3(0, 0, -5));
	this->mats.view = glm::mat4(1.f);
	this->mats.proj = glm::mat4(1.f);

	this->prevCamMat = SetToMatTranslation(this->transform.position);
	this->UpdateViewMatrix();
}

Camera::~Camera() {}

void Camera::Update(float deltaTime)
{
	this->CheckForInput();
	this->UpdateViewMatrix();
	this->UpdateProjectionMatrix(); // Projection

	if (!updatedCameraProjPos) {
		this->updatedCameraProjPos = true;
		this->UpdateViewMatrix();
	}

	Object::Update(deltaTime);
}

void Camera::CheckForInput()
{
}

void Camera::UpdateViewMatrix()
{
	glm::mat4 worldCam;
	glm::mat4 temp;

	// Rotation
	worldCam = glm::mat4(1.f);
	temp = glm::mat4(1.f);
	temp = SetToMatRotationX(this->transform.rotation.x);
	worldCam *= temp;

	temp = glm::mat4(1.f);
	temp = SetToMatRotationY(this->transform.rotation.y);
	worldCam *= temp;

	// Translation
	if (this->camType < 2)
		this->transform.position = GetPosFromMat(prevCamMat) + GetZDirFromMat(worldCam) * (this->forward * moveSpeed);
	else
		this->transform.position = GetPosFromMat(prevCamMat) + GetYDirFromMat(worldCam) * (this->forward * moveSpeed);
	this->transform.position += GetXDirFromMat(worldCam) * (this->rightward * moveSpeed);

	// Update Camera Rot and Translation
	worldCam = SetToMatTranslation(this->transform.position);
	prevCamMat = worldCam;
	worldCam = Inverse(worldCam);
	this->mats.view = worldCam;
}

void Camera::UpdateProjectionMatrix()
{
	/*int width = (viewport.right - viewport.left);
	int height = (viewport.bottom - viewport.top);*/
	int width = 1920;
	int height = 1080;

	switch (type) {
	case orthographic:
	case topdown:
	case sidescroller:
		this->mats.proj = glm::ortho(
			0.f, width / 100.0f,
			0.f, height / 100.0f,
			NEAR_PLANE, FAR_PLANE
		);
		break;

	case perspective:
		this->mats.proj = glm::perspective(
			1.57f, // fov
			(float)height / (float)width, // aspect
			NEAR_PLANE, // near
			FAR_PLANE // far
		);
		break;

	default:
		this->mats.proj = glm::ortho(
			0.f, width / 100.0f,
			0.f, height / 100.0f,
			NEAR_PLANE, FAR_PLANE
		);
		break;
	}
}

void Camera::Move(glm::vec3 offset)
{
	this->transform.position += offset;
	this->CalculateWorldMatrix();
}

float Camera::GetPanSpeed()
{
	return this->panSpeed;
}

glm::mat4 Camera::GetViewMatrix()
{
	return this->mats.view;
}

glm::mat4 Camera::GetProjMatrix()
{
	return this->mats.proj;
}

glm::vec3 Camera::GetCameraCenter()
{
	return this->center;
}

void Camera::SetCenterOffset(glm::vec3 offset)
{
	this->center += offset;
	this->CalculateWorldMatrix();
}

void Camera::SetForward(float dir)
{
	this->forward = dir;
}

void Camera::SetRightward(float dir)
{
	this->rightward = dir;
}

void Camera::SetProjectionType(int type)
{
	this->type = type;
	this->updatedCameraProjPos = false;
}