#pragma once

#include <glm/glm.hpp>

class Light
{
public:
	Light();
	Light(const glm::vec3& color);
	virtual ~Light();

	const glm::vec3& GetColor()  const;
	void SetColor(const glm::vec3& color);

protected:
	glm::vec3 color;
};
