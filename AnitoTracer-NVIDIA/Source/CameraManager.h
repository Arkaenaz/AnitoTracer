#pragma once
#include <vector>

#include "Camera.h"

class Camera;

class CameraManager
{
public:
	static CameraManager* getInstance();
	static void Init();
	static void Destroy();

	void Update(float deltaTime);

	void AddCamera(Camera::CameraType type, int count = 1);
	void RemoveCamera(Camera* cam);

	Camera* GetActiveCamera();
	glm::mat4 GetActiveCameraView();
	glm::mat4 GetActiveProjection();

	void SetSelectedCameraIndex(int select);

private:
	CameraManager();
	~CameraManager();
	CameraManager(CameraManager const&) {};
	CameraManager& operator = (CameraManager const&) {};
	static CameraManager* sharedInstance;

	std::vector<Camera*> CameraList;
	int selectedCameraIndex = 0;
};