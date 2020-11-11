#pragma once
#include "Scene.h"
#include "vector"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(const Scene& scene);
	double FindMaxXorYPointForScaleFactor(MeshModel& currentModel);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void Renderer::ScaleLocal(const Scene& scene, const float scaleFactor);
	
private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void Renderer::DrawTriangle(const std::vector<glm::vec3>& vertexPositions, float localScale, float localRotAngle, glm::vec2 localPosition,
		float worldScale, float worldRotAngle, glm::vec2 worldPosition);

	void CreateBuffers(int w, int h);
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	void Swap(int& X1, int& Y1, int& X2, int& Y2);

	float* color_buffer_;
	int viewport_width_;
	int viewport_height_;
	GLuint gl_screen_tex_;
	GLuint gl_screen_vtc_;
};
