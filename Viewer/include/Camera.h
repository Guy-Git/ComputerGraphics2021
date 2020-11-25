#pragma once
#include <glm/glm.hpp>
#include <glm\ext\matrix_transform.hpp>
#include <glm\ext\matrix_clip_space.hpp>


class Camera
{
public:
	Camera();
	virtual ~Camera();

	void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);
	glm::mat4 Camera::GetCameraLookAt();
	const glm::mat4x4& GetProjectionTransformation() const;
	void Camera::SetOrthographicTrans(float left, float right, float bottom, float top, float near, float far);
	glm::mat4 Camera::GetOrthographicTrans();
	void Camera::SetPerspectiveTrans(float fov, float aspectRatio, float near, float far);
	glm::mat4 Camera::GetPerspectiveTrans();
	void Camera::ResetProjectionsMatrix();
	void Camera::ResetPerspectiveTrans();

	void Camera::SetCameraEye(glm::vec3 cameraEye);
	glm::vec3 Camera::GetCameraEye();

	void Camera::SetCameraAt(glm::vec3 cameraAt);
	glm::vec3 Camera::GetCameraAt();

	void Camera::SetCameraUp(glm::vec3 cameraUp);
	glm::vec3 Camera::GetCameraUp();

	void Camera::ResetCameraPosition();


private:
	glm::mat4x4 view_transformation_;
	glm::mat4x4 projection_transformation_;
	glm::mat4x4 orthographic_transformation_;
	glm::mat4x4 perspective_transformation_;
	glm::mat4x4 zoom_transformation_;

	glm::vec3 cameraEye_;
	glm::vec3 cameraAt_;
	glm::vec3 cameraUp_;
};