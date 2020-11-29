#include "Camera.h"

Camera::Camera()
{
	//orthographic_transformation_ = glm::mat4(1);
	perspective_transformation_ = glm::mat4(1);
	view_transformation_ = glm::mat4(1);

	cameraEye_ = glm::vec3(0.0f, 0.0f, 450.0f);
	cameraAt_ = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraUp_ = glm::vec3(0.0f, 1.0f, 0.0f);
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
}

glm::mat4 Camera::GetPerspectiveTrans()
{
	return (perspective_transformation_);
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

void Camera::SetCameraEye(glm::vec3 cameraEye)
{
	cameraEye_ = cameraEye;
}

glm::vec3 Camera::GetCameraEye()
{
	return cameraEye_;
}

void Camera::SetCameraAt(glm::vec3 cameraAt)
{
	cameraAt_ = cameraAt;
}

glm::vec3 Camera::GetCameraAt()
{
	return cameraAt_;
}

void Camera::SetCameraUp(glm::vec3 cameraUp)
{
	cameraUp_ = -cameraUp;
}

glm::vec3 Camera::GetCameraUp()
{
	return cameraUp_;
}

glm::mat4 Camera::GetCameraLookAt()
{
	return (view_transformation_);
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	view_transformation_ = (glm::lookAt(eye, at, up));
}

void Camera::ResetCameraPosition()
{
	cameraEye_ = glm::vec3(0.0f, 0.0f, 450.0f);
	cameraAt_ = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraUp_ = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::setSelfAngle(float newAngle)
{
	selfAngle_ = newAngle;
}

float Camera::getSelfAngle()
{
	return glm::radians(selfAngle_);
}