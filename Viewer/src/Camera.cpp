#include "Camera.h"

Camera::Camera()
{
	
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