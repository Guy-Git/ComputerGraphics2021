#include "PointLight.h"
#include <algorithm>

PointLight::PointLight(const glm::vec3& position)
{
}

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color)
{

}

PointLight::~PointLight()
{
}

glm::vec3& PointLight::GetPosition()
{
	return position;
}