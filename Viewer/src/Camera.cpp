#include "Camera.h"

Camera::Camera()
{
	orthographic_transformation_ = glm::mat4(1);
	perspective_transformation_ = glm::mat4(1);
}

Camera::~Camera()
{
	
}

const glm::mat4x4& Camera::GetProjectionTransformation() const
{
	return projection_transformation_;
}

const glm::mat4x4& Camera::GetViewTransformation() const
{
	return view_transformation_;
}

void Camera::SetOrthographicTrans(float left, float right, float bottom, float top, float near, float far)
{
	orthographic_transformation_ = glm::mat4((2 / right - left), 0, 0, 0,
		0, 2 / (top - bottom), 0, 0,
		0, 0, 2 / (near - far), 0,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(near + far) / (near - far), 1);
}

glm::mat4 Camera::GetOrthographicTrans()
{
	return orthographic_transformation_;
}

void Camera::SetPerspectiveTrans(float left, float right, float bottom, float top, float near, float far)
{
	perspective_transformation_ = glm::mat4((2 * near)/(right-left), 0, 0, 0,
		0, (2 * near)/(top - bottom), 0, 0,
		(right + left)/(right - left), (top + bottom) / (top - bottom), -(far + near)/(far - near), -1,
		0, 0, -(2 * near * far) / (far - near), 0);
}

glm::mat4 Camera::GetPerspectiveTrans()
{
	return perspective_transformation_;
}

void Camera::ResetOrthographicTrans()
{
	orthographic_transformation_ = glm::mat4(1);
}

void Camera::ResetPerspectiveTrans()
{
	perspective_transformation_ = glm::mat4(1);
}