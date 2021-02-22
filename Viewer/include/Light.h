#pragma once

#include <glm/glm.hpp>

class Light
{
public:
	Light();
	virtual ~Light();

	const glm::vec3& GetColor()  const;
	void SetColor(const glm::vec3& color);

	const glm::vec3& Light::GetPos() const;
	void Light::SetPos(const glm::vec3& pos);

protected:
	glm::vec3 color;
	glm::vec3 pos;
};
