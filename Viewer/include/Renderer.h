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
	void Render(Scene& scene);
	double FindMaxXorYPointForScaleFactor(MeshModel& currentModel);
	void FindMaxValues(const std::vector<glm::vec3>& triangle);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void SetViewport(int height, int width);
	void Renderer::ScaleLocal(const Scene& scene, const float scaleFactor);

private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void Renderer::DrawTriangle(const std::vector<glm::vec3>& vertexPositions, int faceID, MeshModel& currentModel);
	glm::mat4 Renderer::Transformations(const std::vector<glm::vec3>& vertexPositions, float localScale, glm::vec3 localRotAngle, glm::vec3 localPosition,
		float worldScale, glm::vec3 worldRotAngle, glm::vec3 worldPosition);
	std::vector<glm::vec3> Renderer::CalcNewPoints(const std::vector<glm::vec3>& vertexPositions, glm::mat4 transformation, Camera& cam, Scene& scene);
	void Renderer::DrawVertexNormals(const std::vector<glm::vec3>& vertexPositions);
	void DrawFaceNormals(const std::vector<glm::vec3>& vertexPositions);
	glm::vec3 CalcNormal(const std::vector<glm::vec3>& vertexPositions);
	void DrawBoundingBox(MeshModel& model, glm::mat4 transformation, const Scene& scene, Camera& cam);
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
