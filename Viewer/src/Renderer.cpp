#include "Renderer.h"
#include "MeshModel.h"
#include "ShaderProgram.h"
#include <imgui/imgui.h>
#include <vector>
#include <memory>
#include <algorithm>


float maxLightXValue = -INFINITY;
float maxLightYValue = -INFINITY;
float maxLightZValue = -INFINITY;

float minLightXValue = INFINITY;
float minLightYValue = INFINITY;
float minLightZValue = INFINITY;

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Render(const std::shared_ptr<Scene>& scene)
{
	int cameraCount = scene->GetCameraCount();
	if (cameraCount > 0)
	{
		int modelCount = scene->GetModelCount();
		const Camera& camera = scene->GetActiveCamera();

		//Light:
		Light currentLight = scene->GetActiveLight();
		glm::vec3 lightPos = currentLight.GetPos();
		glm::vec3 lightColor = currentLight.GetColor();

		//Model:
		for (int currentModelIndex = 0; currentModelIndex < modelCount; currentModelIndex++)
		{
			std::shared_ptr<MeshModel> currentModel = scene->GetModel(currentModelIndex);

			// Activate the 'colorShader' program (vertex and fragment shaders)
			colorShader.use();

			// Set the uniform variables
			colorShader.setUniform("model", currentModel->GetWorldTransformation() * currentModel->GetModelTransformation());
			colorShader.setUniform("view", camera.GetViewTransformation());
			colorShader.setUniform("projection", camera.GetProjectionTransformation());
			colorShader.setUniform("lightPos", lightPos);
			colorShader.setUniform("material.textureMap", 0);

			// Set 'texture1' as the active texture at slot #0
			texture1.bind(0);

			// Drag our model's faces (triangles) in fill mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindVertexArray(currentModel->GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, currentModel->GetModelVertices().size());
			glBindVertexArray(0);

			// Unset 'texture1' as the active texture at slot #0
			texture1.unbind(0);

			colorShader.setUniform("lightColor", lightColor);

			colorShader.setUniform("modelOrLight", 1);
			colorShader.setUniform("modelColor", currentModel->GetColor());
		}
	}
}

void Renderer::LoadShaders()
{
	colorShader.loadShaders("vshader.glsl", "fshader.glsl");
}

void Renderer::LoadTextures()
{
	/*if (!texture1.loadTexture("bin\\Debug\\crate.jpg", true))
	{
		texture1.loadTexture("bin\\Release\\crate.jpg", true);
	}*/
}

void Renderer::FindMaxLightValues(const std::vector<glm::vec4>& triangle)
{
	for (int i = 0; i < triangle.size(); i++)
	{
		if (triangle.at(i).x > maxLightXValue)
		{
			maxLightXValue = triangle.at(i).x;
		}
		if (triangle.at(i).y > maxLightYValue)
		{
			maxLightYValue = triangle.at(i).y;
		}
		if (triangle.at(i).z > maxLightZValue)
		{
			maxLightZValue = triangle.at(i).z;
		}

		if (triangle.at(i).x < minLightXValue)
		{
			minLightXValue = triangle.at(i).x;
		}
		if (triangle.at(i).y < minLightYValue)
		{
			minLightYValue = triangle.at(i).y;
		}
		if (triangle.at(i).z < minLightZValue)
		{
			minLightZValue = triangle.at(i).z;
		}
	}
}