#include "Light.h"

Light::Light() :
	color(glm::vec3(1.0, 1.0, 1.0)),
	pos(glm::vec3(0.0, 0.0, 0.0))
{

}


Light::~Light()
{
}

const glm::vec3& Light::GetColor() const
{
	return color;
}

void Light::SetColor(const glm::vec3& color)
{
	this->color = color;
}

const glm::vec3& Light::GetPos() const
{
	return pos;
}

void Light::SetPos(const glm::vec3& pos)
{
	this->pos += pos;
}