#include "Scene.h"
#include "MeshModel.h"
#include <string>

Scene::Scene() :
	active_camera_index_(0),
	active_model_index_(0),
	active_light_index_(0),
	scaleFactor_(1),
	rotateAngle_(0),
	position_(0),
	window_height_(900.0),
	window_width_(1500.0)
{
	AddCamera(*(new Camera()));
	AddCamera(*(new Camera()));
}

void Scene::AddModel(const std::shared_ptr<MeshModel>& mesh_model)
{
	mesh_models_.push_back(mesh_model);
}

void Scene::AddLight(const std::shared_ptr<MeshModel>& mesh_model)
{
	lights_.push_back(mesh_model);
}

int Scene::GetLightCount() const
{
	return lights_.size();
}

MeshModel& Scene::GetLight(int index) const
{
	return *lights_[index];
}

MeshModel& Scene::GetActiveLight() const
{
	return *lights_[active_light_index_];
}

int Scene::GetModelCount() const
{
	return mesh_models_.size();
}

MeshModel& Scene::GetModel(int index) const
{
	return *mesh_models_[index];
}

MeshModel& Scene::GetActiveModel() const
{
	return *mesh_models_[active_model_index_];
}

void Scene::AddCamera(const Camera& camera)
{
	cameras_.push_back(camera);
}

int Scene::GetCameraCount() const
{
	return cameras_.size();
}

Camera& Scene::GetCamera(int index)
{
	return cameras_[index];
}

Camera& Scene::GetActiveCamera()
{
	return cameras_[active_camera_index_];
}

void Scene::SetActiveCameraIndex(int index)
{
	active_camera_index_ = index;
}

void Scene::SetActiveLightIndex(int index)
{
	active_light_index_ = index;
}

int Scene::GetActiveCameraIndex() const
{
	return active_camera_index_;
}

void Scene::SetActiveModelIndex(int index)
{
	active_model_index_ = index;
}

int Scene::GetActiveModelIndex() const
{
	return active_model_index_;
}

void Scene::SetScaleFactor(float scaleFactor)
{
	Scene::scaleFactor_ = scaleFactor;
}

float Scene::GetScaleFactor() const
{
	return Scene::scaleFactor_;
}

void Scene::SetRotateAngle(glm::vec3 rotateAngle)
{
	Scene::rotateAngle_ = rotateAngle;
}

glm::vec3 Scene::GetRotateAngle() const
{
	return Scene::rotateAngle_;
}

void Scene::SetNewPosition(glm::vec3 newPos)
{
	position_ = newPos;
}

glm::vec3 Scene::GetPosition() const
{
	return position_;
}

float Scene::GetWidth() const
{
	return window_width_;
}

float Scene::GetHeight() const
{
	return window_height_;
}

void Scene::SetWindowSizes(int height, int width)
{
	window_width_ = width;
	window_height_ = height;
}