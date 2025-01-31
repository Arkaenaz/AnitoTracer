#include "CameraManager.h"

CameraManager* CameraManager::sharedInstance = nullptr;
CameraManager* CameraManager::getInstance()
{
	return sharedInstance;
}

void CameraManager::Init()
{
	sharedInstance = new CameraManager();
}

void CameraManager::Destroy()
{
	delete sharedInstance;
}

void CameraManager::Update(float deltaTime)
{
	for (Camera* cam : this->CameraList)
	{
		if (cam->GetActive())
			cam->Update(deltaTime);
	}
}

void CameraManager::AddCamera(Camera::CameraType type, int count)
{
	for (int i = 0; i < count; i++) {
		Camera* newCam = new Camera("Camera " + std::to_string(this->CameraList.size() + 1), type);
		this->CameraList.push_back(newCam);

		// TODO: Add Listener to Camera.
		//InputSystem::getInstance()->addListener(newCam);
	}
}

void CameraManager::RemoveCamera(Camera* cam)
{
	// TODO: Remove Listener from Camera.
	//InputSystem::getInstance()->removeListener(cam);

	std::vector<Camera*>::iterator it;

	for (it = this->CameraList.begin(); it != this->CameraList.end(); ++it) {
		if (*it == cam)
			this->CameraList.erase(it);
	}
}

Camera* CameraManager::GetActiveCamera()
{
	return this->CameraList[this->selectedCameraIndex];
}

glm::mat4 CameraManager::GetActiveCameraView()
{
	return this->CameraList[this->selectedCameraIndex]->GetViewMatrix();
}

glm::mat4 CameraManager::GetActiveProjection()
{
	return this->CameraList[this->selectedCameraIndex]->GetProjMatrix();
}

void CameraManager::SetSelectedCameraIndex(int select)
{
	this->selectedCameraIndex = select;
}

CameraManager::CameraManager()
{
	this->AddCamera(Camera::CameraType::perspective);
}

CameraManager::~CameraManager()
{
	this->CameraList.clear();
}