#pragma once
#include "Scene.h"
#include "vector"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ShaderProgram.h>
#include <Texture2D.h>

class Renderer
{
public:
	//Renderer(int viewportWidth, int viewportHeight);
	Renderer::Renderer();
	virtual ~Renderer();
	void Render(Scene& scene);
	double FindMaxXorYPointForScaleFactor(MeshModel& currentModel);
	void FindMaxValues(const std::vector<glm::vec3>& triangle);
	void FindMaxLightValues(const std::vector<glm::vec3>& triangle);
	//void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	//int GetViewportWidth() const;
	//int GetViewportHeight() const;
	//void SetViewport(int height, int width, glm::vec3 color);
	void Renderer::ScaleLocal(const Scene& scene, const float scaleFactor);

	glm::vec3 CalcColorOfFace(Scene& scene, glm::vec3 faceNormal, glm::vec3 lightPoint, glm::vec3 modelPoint);

	void LoadShaders();
	void LoadTextures();

private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void Renderer::DrawGouraudTriangle(const std::vector<glm::vec3>& vertexPositions, MeshModel& currentModel, Scene& scene, glm::vec3 lightPoint, std::vector <glm::vec3> vertexNormals);
	void DrawFlatTriangle(const std::vector<glm::vec3>& vertexPositions, MeshModel& currentModel, Scene& scene, glm::vec3 lightPoint, glm::vec3 faceNormal);
	void DrawPhongTriangle(std::vector<glm::vec3>& vertexPositions, MeshModel& currentModel, Scene& scene, glm::vec3 lightPoint, std::vector<glm::vec3> vertexNormals);
	void DrawLightTriangle(const std::vector<glm::vec3>& vertexPositions, int faceID, MeshModel& currentModel, Scene& scene, glm::vec3 color);
	glm::mat4 Renderer::Transformations(float localScale, glm::vec3 localRotAngle, glm::vec3 localPosition,
		float worldScale, glm::vec3 worldRotAngle, glm::vec3 worldPosition);

	glm::mat4 Renderer::LightTransformations(const std::vector<glm::vec3>& vertexPositions, float localScale, glm::vec3 localPosition);

	glm::mat4 CameraTransformation(Camera& cam, Scene& scene);

	std::vector<glm::vec3> Renderer::CalcNewPoints(const std::vector<glm::vec3>& vertexPositions, glm::mat4 transformation, Camera& cam, Scene& scene);
	void Renderer::updatePosition(glm::mat4 transformation, Camera& cam, Scene& scene, bool isModelOrLight);
	glm::vec3 Renderer::DrawVertexNormals(const std::vector<glm::vec3>& vertexPositions, bool drawOrReturn);
	glm::vec3 DrawFaceNormals(const std::vector<glm::vec3>& vertexPositions, bool drawOrReturn);
	glm::vec3 CalcNormal(const std::vector<glm::vec3>& vertexPositions);
	glm::vec3 DrawBoundingBox(MeshModel& model, glm::mat4 transformation, const Scene& scene, Camera& cam);
	float Area(float dX0, float dY0, float dX1, float dY1, float dX2, float dY2);
	glm::vec3 GouraudColor(std::vector<glm::vec3>& vertexColors, float dX0, float dY0, float dX1, float dY1, float dX2, float dY2, float px, float py);
	glm::vec3 PhongNormalInterpolation(std::vector<glm::vec3>& vertexNormals, std::vector<glm::vec3>& vertexPositions, float px, float py);
	void DrawNormal(const glm::vec3 vertexPosition, glm::vec3 normal);
	void PostProcessingFunctions(Scene scene);
	void CombineBlooming(glm::vec3** BrightColor, float bloomTreshold);
	glm::vec3 BloomTreshold();
	glm::vec3** Renderer::ApplyGaussianBlur(glm::vec3** BrightColorSrc, glm::vec3** BrightColorDst);
	//void CreateBuffers(int w, int h);
	//void CreateOpenGLBuffer();
	//void InitOpenGLRendering();
	void Swap(int& X1, int& Y1, int& X2, int& Y2);

	float* color_buffer_;
	int viewport_width_;
	int viewport_height_;

	ShaderProgram lightShader;
	ShaderProgram colorShader;
	Texture2D texture1;
};
