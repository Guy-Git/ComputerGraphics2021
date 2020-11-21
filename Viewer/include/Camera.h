#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera();
	virtual ~Camera();

	void SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);

	const glm::mat4x4& GetProjectionTransformation() const;
	const glm::mat4x4& GetViewTransformation() const;
	void Camera::SetOrthographicTrans(float left, float right, float bottom, float top, float near, float far);
	glm::mat4 Camera::GetOrthographicTrans();
	void Camera::SetPerspectiveTrans(float left, float right, float bottom, float top, float near, float far);
	glm::mat4 Camera::GetPerspectiveTrans();
	void Camera::ResetOrthographicTrans();
	void Camera::ResetPerspectiveTrans();

private:
	glm::mat4x4 view_transformation_;
	glm::mat4x4 projection_transformation_;
	glm::mat4x4 orthographic_transformation_;
	glm::mat4x4 perspective_transformation_;
	glm::mat4x4 zoom_transformation_;

};
