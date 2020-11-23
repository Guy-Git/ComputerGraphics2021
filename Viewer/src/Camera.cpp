#include "Camera.h"

Camera::Camera()
{
	orthographic_transformation_ = glm::mat4(1);
	perspective_transformation_ = glm::mat4(1);
	view_transformation_ = glm::mat4(1);

	/*view_transformation_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));*/
}

Camera::~Camera()
{

}

const glm::mat4x4& Camera::GetProjectionTransformation() const
{
	return projection_transformation_;
}

void Camera::SetOrthographicTrans(float left, float right, float bottom, float top, float near, float far)
{
	orthographic_transformation_ = glm::ortho(left, right, bottom, top, near, far);
}

glm::mat4 Camera::GetOrthographicTrans()
{
	return orthographic_transformation_;
}

void Camera::SetPerspectiveTrans(float fov, float aspectRatio, float near, float far)
{
	perspective_transformation_ = glm::perspective(glm::radians(fov), aspectRatio, near, far);

	/*float nearHeight = 2 * near * tan(0.5 * fov * 3.14 / 180);
	float num = 1 / tan(0.5 * fov * 3.14 / 180);
	float nearWidth = aspectRatio * nearHeight;
	float t = 0.5 * nearHeight;
	float b = -0.5 * nearHeight;
	float l = -0.5 * nearWidth;
	float r = 0.5 * nearWidth;
	glm::vec4 v1((2* near )/ (r-l), 0, 0, 0);
	glm::vec4 v2(0, (2*near) / (t-b), 0, 0);
	glm::vec4 v3((r+l)/(r-l), (t+b)/(t-b), -1 * (far + near) / (far - near), -1);
	glm::vec4 v4(0, 0, -2 * (far * near) / (far - near), 0);
	perspective_transformation_ = glm::mat4(v1, v2, v3, v4);*/
}

glm::mat4 Camera::GetPerspectiveTrans()
{
	return perspective_transformation_;
}

void Camera::ResetProjectionsMatrix()
{
	orthographic_transformation_ = glm::mat4(1);
	view_transformation_ = glm::mat4(1);
	perspective_transformation_ = glm::mat4(1);
}

void Camera::ResetPerspectiveTrans()
{
	perspective_transformation_ = glm::mat4(1);
}

glm::mat4 Camera::GetCameraLookAt()
{
	return (view_transformation_);
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	view_transformation_ = (glm::lookAt(eye, at, up));
}