#pragma once

#include <vector>
#include <memory>

#include "Camera.h"
#include "MeshModel.h"

class Scene {
public:
	Scene();

	void AddModel(const std::shared_ptr<MeshModel>& mesh_model);
	void Scene::AddLight(const std::shared_ptr<MeshModel>& mesh_model);

	int GetModelCount() const;
	MeshModel& GetModel(int index) const;
	MeshModel& GetActiveModel() const;

	int GetLightCount() const;
	MeshModel& GetLight(int index) const;
	MeshModel& GetActiveLight() const;
	void Scene::SetActiveLightIndex(int index);
	
	void Scene::AddCamera(const Camera& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	Camera& GetActiveCamera();

	void SetActiveCameraIndex(int index);
	int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	int GetActiveModelIndex() const;

	float Scene::GetScaleFactor() const;
	void SetScaleFactor(float scaleFactor);
	void SetRotateAngle(glm::vec3 rotateAngle);
	glm::vec3 GetRotateAngle() const;
	void SetNewPosition(glm::vec3 newPos);
	glm::vec3 GetPosition() const;

	void SetWindowSizes(int& height, int& width);
	float Scene::GetWidth() const;
	float Scene::GetHeight() const;

	void SetPPMode(int mode);
	int GetPPMode();

	void RemoveLight();
	
private:
	std::vector<std::shared_ptr<MeshModel>> mesh_models_;
	std::vector<std::shared_ptr<MeshModel>> lights_;
	std::vector<Camera> cameras_;

	int active_camera_index_;
	int active_model_index_;
	int active_light_index_;

	float scaleFactor_;
	glm::vec3 rotateAngle_;
	glm::vec3 position_;
	

	float window_width_;
	float window_height_;

	int PPmode_; // 0 = none, 1 = bloom + gaussian, 2 = TBD
};