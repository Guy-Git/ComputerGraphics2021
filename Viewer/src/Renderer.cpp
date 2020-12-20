#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "vector"
#include "Renderer.h"
#include "InitShader.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <GLFW\glfw3.h>

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

double maxPointXValue = -INFINITY;
double maxPointYValue = -INFINITY;
double maxPointZValue = -INFINITY;

double minPointXValue = INFINITY;
double minPointYValue = INFINITY;
double minPointZValue = INFINITY;

float** zBuff;

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width_(viewport_width),
	viewport_height_(viewport_height)
{
	InitOpenGLRendering();
	CreateBuffers(viewport_width, viewport_height);
}

Renderer::~Renderer()
{
	delete[] color_buffer_;
}

void Renderer::SetViewport(int height, int width)
{
	viewport_width_ = width;
	viewport_height_ = height;

	CreateBuffers(viewport_width_, viewport_height_);
	CreateOpenGLBuffer();
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width_) return;
	if (j < 0) return; if (j >= viewport_height_) return;

	color_buffer_[INDEX(viewport_width_, i, j, 0)] = color.x;
	color_buffer_[INDEX(viewport_width_, i, j, 1)] = color.y;
	color_buffer_[INDEX(viewport_width_, i, j, 2)] = color.z;
}

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	int X1, Y1, X2, Y2, C, E;

	X1 = p1.x;
	Y1 = p1.y;
	X2 = p2.x;
	Y2 = p2.y;
	E = -1;

	int validatePoint = 30000;
	if (X1 > validatePoint || X1 < -validatePoint || X2 > validatePoint || X2 < -validatePoint ||
		Y1 > validatePoint || Y1 < -validatePoint || Y2 > validatePoint || Y2 < -validatePoint)
	{
		return;
	}

	int deltaX = X2 - X1;
	int deltaY = Y2 - Y1;

	if (X1 == X2)
	{
		while (Y1 <= Y2)
		{
			PutPixel(X1, Y1, color);
			Y1 += 1;
		}
		while (Y2 <= Y1)
		{
			PutPixel(X1, Y1, color);
			Y1 -= 1;
		}
		return;
	}

	float slope = (float)deltaY / (float)deltaX;

	C = Y1 - (slope * X1);

	if (slope == 0)
	{
		if (X1 > X2)
			Swap(X1, Y1, X2, Y2);

		while (X1 <= X2)
		{
			PutPixel(X1, Y1, color);

			X1 += 1;
		}
	}
	else if (slope == 1)
	{
		if (X1 > X2)
			Swap(X1, Y1, X2, Y2);

		while (X1 <= X2)
		{
			PutPixel(X1, Y1, color);

			Y1 += 1;
			X1 += 1;
		}
	}

	else if (slope == -1)
	{
		if (Y2 > Y1)
			Swap(X1, Y1, X2, Y2);

		while (Y2 <= Y1)
		{
			PutPixel(X1, Y1, color);

			Y1 -= 1;
			X1 += 1;
		}
	}

	else if (slope > 0)
	{
		if (X1 > X2)
		{
			Swap(X1, Y1, X2, Y2);
			deltaX *= -1;
			deltaY *= -1;
		}

		E = (2 * deltaY * X1) + (2 * deltaX * C) - (2 * deltaX * Y1) - 1;

		if (slope < 1)
		{
			while (X1 <= X2)
			{
				if (E > 0)
				{
					Y1 += 1;
					E -= 2 * deltaX;
				}

				PutPixel(X1, Y1, color);

				X1 += 1;
				E += 2 * deltaY;
			}
		}
		else if (slope > 1)
		{
			while (Y1 <= Y2)
			{
				if (E > 0)
				{
					X1 += 1;
					E -= 2 * deltaY;
				}

				PutPixel(X1, Y1, color);

				Y1 += 1;
				E += 2 * deltaX;
			}
		}

	}

	else if (slope < 0)
	{
		if (Y1 < Y2)
		{
			Swap(X1, Y1, X2, Y2);
			deltaX *= -1;
			deltaY *= -1;
		}

		E = (2 * deltaY * X1) + (2 * deltaX * C) - (2 * deltaX * Y1) - 1;

		if (slope > -1)
		{
			while (X1 <= X2)
			{
				if (E > 0)
				{
					Y1 -= 1;
					E -= 2 * deltaX;
				}

				PutPixel(X1, Y1, color);

				X1 += 1;
				E -= 2 * deltaY;
			}
		}

		else if (slope < -1)
		{
			while (Y1 >= Y2)
			{
				if (E > 0)
				{
					X1 += 1;
					E += 2 * deltaY;
				}

				PutPixel(X1, Y1, color);

				Y1 -= 1;
				E += 2 * deltaX;
			}
		}
	}
}


void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenGLBuffer(); //Do not remove this line.
	color_buffer_ = new float[3 * w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex_);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc_);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc_);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)__(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[] = {
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[] = {
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1 };

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(vtc));

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
}

void Renderer::CreateOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width_, viewport_height_, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width_, viewport_height_);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width_, viewport_height_, GL_RGB, GL_FLOAT, color_buffer_);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc_);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width_; i++)
	{
		for (int j = 0; j < viewport_height_; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

void Renderer::Render(Scene& scene)
{
	DrawLine(glm::ivec2(0, scene.GetHeight() / 2), glm::ivec2(scene.GetWidth(), scene.GetHeight() / 2), glm::vec3(0, 0, 0)); // X axis

	DrawLine(glm::ivec2(scene.GetWidth() / 2, scene.GetHeight()), glm::ivec2(scene.GetWidth() / 2, 0), glm::vec3(0, 0, 0)); // Y axis

	//Creation
	zBuff = new float* [scene.GetWidth()]; // Rows

	for (int i = 0; i < scene.GetWidth(); i++)
	{
		zBuff[i] = new float[scene.GetHeight()]; // Columns
	}

	for (int i = 0; i < scene.GetWidth(); i++)
		for (int j = 0; j < scene.GetHeight(); j++)
			zBuff[i][j] = -INFINITY;

	if (scene.GetLightCount() > 0)
	{
		for (int i = 0; i < scene.GetLightCount(); i++)
		{
			MeshModel currentLight = scene.GetLight(i);
			double scaleFactor = abs(100 / FindMaxXorYPointForScaleFactor(currentLight));
			std::vector <glm::vec3> threePoints;
			std::vector <glm::vec3> threePointsAfterTransformations;
			glm::mat4 transformationMatrix;

			for (int i = 0; i < currentLight.GetFacesCount(); i++)
			{
				threePoints.push_back(currentLight.GetVertex(currentLight.GetFace(i).GetVertexIndex(0) - 1));
				threePoints.push_back(currentLight.GetVertex(currentLight.GetFace(i).GetVertexIndex(1) - 1));
				threePoints.push_back(currentLight.GetVertex(currentLight.GetFace(i).GetVertexIndex(2) - 1));

				FindMaxValues(threePoints);

				transformationMatrix = LightTransformations(threePoints, scaleFactor * currentLight.GetScaleFactor(), currentLight.GetPosition());
				threePointsAfterTransformations = CalcNewPoints(threePoints, transformationMatrix, scene.GetActiveCamera(), scene);

				DrawTriangle(threePointsAfterTransformations, i, currentLight, scene, scene.GetActiveLight().GetColorOfMesh());

				if (currentLight.isLightRotating_)
				{
					double max = FindMaxXorYPointForScaleFactor(scene.GetActiveModel());

					float lightX = 4.0 * sin(glfwGetTime()) + max;
					float lightY = -0.3f;
					float lightZ = 3.0 * cos(glfwGetTime()) + max;
					currentLight.SetNewPosition(glm::vec3(lightX, lightY, lightZ));
				}

				threePoints.clear();
			}
		}
	}

	if (scene.GetModelCount() > 0)
	{
		for (size_t i = 0; i < scene.GetModelCount(); i++)
		{

			MeshModel currentModel = scene.GetModel(i);
			double scaleFactor = abs(400 / FindMaxXorYPointForScaleFactor(currentModel));
			std::vector <glm::vec3> threePoints;
			std::vector <glm::vec3> threePointsAfterTransformations;
			glm::mat4 transformationMatrix;

			for (int i = 0; i < currentModel.GetFacesCount(); i++)
			{
				threePoints.push_back(currentModel.GetVertex(currentModel.GetFace(i).GetVertexIndex(0) - 1));
				threePoints.push_back(currentModel.GetVertex(currentModel.GetFace(i).GetVertexIndex(1) - 1));
				threePoints.push_back(currentModel.GetVertex(currentModel.GetFace(i).GetVertexIndex(2) - 1));

				FindMaxValues(threePoints);

				transformationMatrix = Transformations(threePoints, scaleFactor * currentModel.GetScaleFactor(), currentModel.GetRotateAngle(), currentModel.GetPosition(),
					scene.GetScaleFactor(), scene.GetRotateAngle(), scene.GetPosition());

				threePointsAfterTransformations = CalcNewPoints(threePoints, transformationMatrix, scene.GetActiveCamera(), scene);

				glm::vec3 faceNormal = glm::vec3(1);
				glm::vec3 vertexNormal = glm::vec3(1);

				if (currentModel.GetVertexNormalShown())
				{
					DrawVertexNormals(threePointsAfterTransformations, true);
				}
				if (currentModel.GetFaceNormalShown())
				{
					DrawFaceNormals(threePointsAfterTransformations);
				}

				vertexNormal = DrawVertexNormals(threePointsAfterTransformations, false);

				glm::vec3 colorOfFace = CalcColorOfFace(scene, vertexNormal, transformationMatrix); // TODO - perspective not working

				DrawTriangle(threePointsAfterTransformations, i, currentModel, scene, colorOfFace);

				threePoints.clear();
			}


			DrawBoundingBox(currentModel, transformationMatrix, scene, scene.GetActiveCamera());


			maxPointXValue = -INFINITY;
			maxPointYValue = -INFINITY;
			maxPointZValue = -INFINITY;

			minPointXValue = INFINITY;
			minPointYValue = INFINITY;
			minPointZValue = INFINITY;
		}
	}

	for (int i = 0; i < scene.GetHeight(); i++)
	{
		delete[] zBuff[i]; // Delete columns
	}
	delete[] zBuff; // Delete Rows
}

glm::vec3 Renderer::CalcColorOfFace(Scene& scene, glm::vec3 normal, glm::mat4 transformationMatrix)
{
	if (scene.GetActiveCamera().GetPerspectiveTrans() != glm::mat4(1))
	{
		normal = glm::mat3(glm::transpose(glm::inverse(transformationMatrix))) * normal;
	}

	if (scene.GetLightCount() > 0)
	{

		float ambientStrength = 0.1;
		glm::vec3 ambient = ambientStrength * scene.GetActiveLight().GetColorOfMesh();

		glm::vec3 lightDir = glm::normalize(scene.GetActiveLight().GetPosition() - scene.GetActiveModel().GetPosition());
		float diff = fmax(glm::dot(normal, lightDir), 0.0);
		glm::vec3 diffuse = diff * scene.GetActiveLight().GetColorOfMesh();
		glm::vec3 lightChange = (ambient + diffuse) * scene.GetActiveModel().GetColorOfMesh();

		return lightChange;
	}
	return scene.GetActiveModel().GetColorOfMesh();
}

double Renderer::FindMaxXorYPointForScaleFactor(MeshModel& currentModel)
{
	// find the max vertex point value to set the scale factor
	double maxPointXValueScale = -INFINITY;
	double maxPointYValueScale = -INFINITY;
	double maxPointZValueScale = -INFINITY;

	for (int i = 0; i < currentModel.GetVerticesCount(); i++)
	{
		if (currentModel.GetVertex(i).x > maxPointXValueScale)
		{
			maxPointXValueScale = currentModel.GetVertex(i).x;
		}
		if (currentModel.GetVertex(i).y > maxPointYValueScale)
		{
			maxPointYValueScale = currentModel.GetVertex(i).y;
		}
		if (currentModel.GetVertex(i).z > maxPointZValueScale)
		{
			maxPointZValueScale = currentModel.GetVertex(i).z;
		}
	}

	double max1 = maxPointXValueScale > maxPointYValueScale ? maxPointXValueScale : maxPointYValueScale;
	double max2 = max1 > maxPointZValueScale ? max1 : maxPointZValueScale;

	return max2;
}

void Renderer::FindMaxValues(const std::vector<glm::vec3>& triangle)
{
	for (int i = 0; i < triangle.size(); i++)
	{
		if (triangle.at(i).x > maxPointXValue)
		{
			maxPointXValue = triangle.at(i).x;
		}
		if (triangle.at(i).y > maxPointYValue)
		{
			maxPointYValue = triangle.at(i).y;
		}
		if (triangle.at(i).z > maxPointZValue)
		{
			maxPointZValue = triangle.at(i).z;
		}

		if (triangle.at(i).x < minPointXValue)
		{
			minPointXValue = triangle.at(i).x;
		}
		if (triangle.at(i).y < minPointYValue)
		{
			minPointYValue = triangle.at(i).y;
		}
		if (triangle.at(i).z < minPointZValue)
		{
			minPointZValue = triangle.at(i).z;
		}
	}
}

int Renderer::GetViewportWidth() const
{
	return viewport_width_;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height_;
}

void Renderer::Swap(int& X1, int& Y1, int& X2, int& Y2)
{
	int tempX = X1;
	int tempY = Y1;
	X1 = X2;
	Y1 = Y2;
	X2 = tempX;
	Y2 = tempY;
}

void Renderer::DrawTriangle(const std::vector<glm::vec3>& vertexPositions, int faceID, MeshModel& currentModel, Scene& scene, glm::vec3 color)
{

	glm::vec3 v1 = vertexPositions.at(0);
	glm::vec3 v2 = vertexPositions.at(1);
	glm::vec3 v3 = vertexPositions.at(2);

	float a1 = v2.x - v1.x;
	float b1 = v2.y - v1.y;
	float c1 = v2.z - v1.z;
	float a2 = v3.x - v1.x;
	float b2 = v3.y - v1.y;
	float c2 = v3.z - v1.z;
	float a = b1 * c2 - b2 * c1;
	float b = a2 * c1 - a1 * c2;
	float c = a1 * b2 - b1 * a2;
	float d = (-a * v1.x - b * v1.y - c * v1.z);
	float z;

	//if() NOT Z-Buffer
	int maxX = fmax(v1.x, fmax(v2.x, v3.x));
	int minX = fmin(v1.x, fmin(v2.x, v3.x));
	int maxY = fmax(v1.y, fmax(v2.y, v3.y));
	int minY = fmin(v1.y, fmin(v2.y, v3.y));

	glm::vec2 vs1 = glm::vec2(v2.x - v1.x, v2.y - v1.y);
	glm::vec2 vs2 = glm::vec2(v3.x - v1.x, v3.y - v1.y);

	/*int randomNum = 1 + (rand() % 9);
	float color0 = float((faceID + randomNum) % 10) / 10.0;
	randomNum = 1 + (rand() % 9);
	float color1 = float((faceID + randomNum) % 10) / 10.0;
	randomNum = 1 + (rand() % 9);
	float color2 = float((faceID + randomNum) % 10) / 10.0;*/

	/*for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			glm::vec2 q = glm::vec2(x - v1.x, y - v1.y);

			float s = (q.x*vs2.y - q.y*vs2.x) / (vs1.x * vs2.y - vs1.y * vs2.x);
			float t = (vs1.x * q.y - vs1.y * q.x) / (vs1.x * vs2.y - vs1.y * vs2.x);

			if ((s >= 0) && (t >= 0) && (s + t <= 1))
			{
				PutPixel(x, y, glm::vec3(color0, color1, color2));
			}
		}
	}


	//else YES Z-Buffer
	*/
	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			glm::vec2 q = glm::vec2(x - v1.x, y - v1.y);
			float s = (q.x * vs2.y - q.y * vs2.x) / (vs1.x * vs2.y - vs1.y * vs2.x);
			float t = (vs1.x * q.y - vs1.y * q.x) / (vs1.x * vs2.y - vs1.y * vs2.x);
			if ((s >= 0) && (t >= 0) && (s + t <= 1))
			{
				z = -(a * x + b * y + d) / c;
				if (x > 0 && y > 0 && y < scene.GetHeight() && x < scene.GetWidth())
				{
					if (z >= zBuff[x][y])
					{
						zBuff[x][y] = z;
						PutPixel(x, y, glm::vec3((zBuff[x][y] - 1.0041) / (1.0052 - 1.0041), (zBuff[x][y] - 1.0041) / (1.0052 - 1.0041), (zBuff[x][y] - 1.0041) / (1.0052 - 1.0041))); // (val - min) / (max - min)
					}
				}
			}
		}
	}
}

glm::mat4 Renderer::Transformations(const std::vector<glm::vec3>& vertexPositions, float localScale, glm::vec3 localRotAngle, glm::vec3 localPosition,
	float worldScale, glm::vec3 worldRotAngle, glm::vec3 worldPosition)
{

	glm::mat4 localScaleMat = glm::mat4(localScale, 0, 0, 0, 0, localScale, 0, 0, 0, 0, localScale, 0, 0, 0, 0, 1);
	glm::mat4 localRotationMatX = glm::mat4(1, 0, 0, 0, 0, cos(localRotAngle.x), sin(localRotAngle.x), 0, 0, -sin(localRotAngle.x), cos(localRotAngle.x), 0, 0, 0, 0, 1);
	glm::mat4 localRotationMatY = glm::mat4(cos(localRotAngle.y), 0, sin(localRotAngle.y), 0, 0, 1, 0, 0, -sin(localRotAngle.y), 0, cos(localRotAngle.y), 0, 0, 0, 0, 1);
	glm::mat4 localRotationMatZ = glm::mat4(cos(localRotAngle.z), sin(localRotAngle.z), 0, 0, -sin(localRotAngle.z), cos(localRotAngle.z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	glm::mat4 localPositionMat = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, localPosition.x, localPosition.y, localPosition.z, 1);

	glm::mat4 worldScaleMat = glm::mat4(worldScale, 0, 0, 0, 0, worldScale, 0, 0, 0, 0, worldScale, 0, 0, 0, 0, 1);
	glm::mat4 worldRotationMatX = glm::mat4(1, 0, 0, 0, 0, cos(worldRotAngle.x), sin(worldRotAngle.x), 0, 0, -sin(worldRotAngle.x), cos(worldRotAngle.x), 0, 0, 0, 0, 1);
	glm::mat4 worldRotationMatY = glm::mat4(cos(worldRotAngle.y), 0, sin(worldRotAngle.y), 0, 0, 1, 0, 0, -sin(worldRotAngle.y), 0, cos(worldRotAngle.y), 0, 0, 0, 0, 1);
	glm::mat4 worldRotationMatZ = glm::mat4(cos(worldRotAngle.z), sin(worldRotAngle.z), 0, 0, -sin(worldRotAngle.z), cos(worldRotAngle.z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	glm::mat4 worldPositionMat = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, worldPosition.x, worldPosition.y, worldPosition.z, 1);

	return worldScaleMat * worldRotationMatX * worldRotationMatY * worldRotationMatZ * worldPositionMat *
		localScaleMat * localRotationMatX * localRotationMatY * localRotationMatZ * localPositionMat;
}

glm::mat4 Renderer::LightTransformations(const std::vector<glm::vec3>& vertexPositions, float localScale, glm::vec3 localPosition)
{
	glm::mat4 localScaleMat = glm::mat4(localScale, 0, 0, 0, 0, localScale, 0, 0, 0, 0, localScale, 0, 0, 0, 0, 1);
	glm::mat4 localPositionMat = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, localPosition.x, localPosition.y, localPosition.z, 1);

	return localScaleMat * localPositionMat;
}

std::vector<glm::vec3> Renderer::CalcNewPoints(const std::vector<glm::vec3>& vertexPositions, glm::mat4 transformation, Camera& cam, Scene& scene)
{
	int x0 = scene.GetWidth() / 2;
	int y0 = scene.GetHeight() / 2;
	int z0 = 0;

	glm::mat4 orthoMat = cam.GetOrthographicTrans();
	glm::mat4 perspecMat = cam.GetPerspectiveTrans();
	glm::mat4 view = cam.GetCameraLookAt();

	glm::mat4 cameraLocalRotationMatX = glm::mat4(1, 0, 0, 0, 0, cos(cam.getSelfAngle().x), sin(cam.getSelfAngle().x), 0, 0, -sin(cam.getSelfAngle().x), cos(cam.getSelfAngle().x), 0, 0, 0, 0, 1);
	glm::mat4 cameraLocalRotationMatY = glm::mat4(cos(cam.getSelfAngle().y), 0, sin(cam.getSelfAngle().y), 0, 0, 1, 0, 0, -sin(cam.getSelfAngle().y), 0, cos(cam.getSelfAngle().y), 0, 0, 0, 0, 1);
	glm::mat4 cameraLocalRotationMatZ = glm::mat4(cos(cam.getSelfAngle().z), sin(cam.getSelfAngle().z), 0, 0, -sin(cam.getSelfAngle().z), cos(cam.getSelfAngle().z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	glm::mat4 cameraWorldRotationMatX = glm::mat4(1, 0, 0, 0, 0, cos(cam.getWorldRotatingAngle().x), sin(cam.getWorldRotatingAngle().x), 0, 0, -sin(cam.getWorldRotatingAngle().x), cos(cam.getWorldRotatingAngle().x), 0, 0, 0, 0, 1);
	glm::mat4 cameraWorldRotationMatY = glm::mat4(cos(cam.getWorldRotatingAngle().y), 0, sin(cam.getWorldRotatingAngle().y), 0, 0, 1, 0, 0, -sin(cam.getWorldRotatingAngle().y), 0, cos(cam.getWorldRotatingAngle().y), 0, 0, 0, 0, 1);
	glm::mat4 cameraWorldRotationMatZ = glm::mat4(cos(cam.getWorldRotatingAngle().z), sin(cam.getWorldRotatingAngle().z), 0, 0, -sin(cam.getWorldRotatingAngle().z), cos(cam.getWorldRotatingAngle().z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	glm::mat4 cameraLocalRotate = cameraLocalRotationMatX * cameraLocalRotationMatY * cameraLocalRotationMatZ;
	glm::mat4 cameraWorldRotate = cameraWorldRotationMatX * cameraWorldRotationMatY * cameraWorldRotationMatZ;


	cameraLocalRotate = glm::inverse(cameraLocalRotate);
	cameraWorldRotate = glm::inverse(cameraWorldRotate);

	float w = 1.0;

	glm::vec4 p1 = glm::vec4((vertexPositions.at(0).x), (vertexPositions.at(0).y), (vertexPositions.at(0).z), w);
	glm::vec4 p2 = glm::vec4((vertexPositions.at(1).x), (vertexPositions.at(1).y), (vertexPositions.at(1).z), w);
	glm::vec4 p3 = glm::vec4((vertexPositions.at(2).x), (vertexPositions.at(2).y), (vertexPositions.at(2).z), w);

	p1 = orthoMat * perspecMat * cameraLocalRotate * view * cameraWorldRotate * transformation * p1;
	p2 = orthoMat * perspecMat * cameraLocalRotate * view * cameraWorldRotate * transformation * p2;
	p3 = orthoMat * perspecMat * cameraLocalRotate * view * cameraWorldRotate * transformation * p3;

	glm::vec3 p1t = glm::vec3(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w);
	glm::vec3 p2t = glm::vec3(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w);
	glm::vec3 p3t = glm::vec3(p3.x / p3.w, p3.y / p3.w, p3.z / p3.w);

	p1t.x += x0; p1t.y += y0; p1t.z += z0;
	p2t.x += x0; p2t.y += y0; p2t.z += z0;
	p3t.x += x0; p3t.y += y0; p3t.z += z0;

	std::vector <glm::vec3> threePoints;
	threePoints.push_back(glm::vec3(p1t.x, p1t.y, p1t.z));
	threePoints.push_back(glm::vec3(p2t.x, p2t.y, p2t.z));
	threePoints.push_back(glm::vec3(p3t.x, p3t.y, p3t.z));

	return threePoints;
}

glm::vec3 Renderer::DrawVertexNormals(const std::vector<glm::vec3>& vertexPositions, bool drawOrReturn)
{
	double scaleFactor = 50;

	glm::vec3 normalVector = CalcNormal(vertexPositions);

	glm::vec3 normalEndPoint = glm::vec3(normalVector.x + vertexPositions.at(0).x,
		normalVector.y + vertexPositions.at(0).y,
		normalVector.z + vertexPositions.at(0).z);

	glm::vec3 scaleEndPoint = glm::vec3(vertexPositions.at(0).x + scaleFactor * (normalEndPoint.x - vertexPositions.at(0).x),
		vertexPositions.at(0).y + scaleFactor * (normalEndPoint.y - vertexPositions.at(0).y),
		vertexPositions.at(0).z + scaleFactor * (normalEndPoint.z - vertexPositions.at(0).z));

	if (drawOrReturn)
	{
		DrawLine(vertexPositions.at(0), scaleEndPoint, glm::vec3(1, 1, 0));
	}

	return normalVector;
}

glm::vec3 Renderer::DrawFaceNormals(const std::vector<glm::vec3>& vertexPositions)
{
	double scaleFactor = 50;

	glm::vec3 triangleCentroid = glm::vec3((vertexPositions.at(0).x + vertexPositions.at(1).x + vertexPositions.at(2).x) / 3,
		(vertexPositions.at(0).y + vertexPositions.at(1).y + vertexPositions.at(2).y) / 3,
		(vertexPositions.at(0).z + vertexPositions.at(1).z + vertexPositions.at(2).z) / 3);

	glm::vec3 normalVector = CalcNormal(vertexPositions);

	glm::vec3 normalEndPoint = glm::vec3(normalVector.x + triangleCentroid.x, normalVector.y + triangleCentroid.y, normalVector.z + triangleCentroid.z);

	glm::vec3 scaleEndPoint = glm::vec3(triangleCentroid.x + scaleFactor * (normalEndPoint.x - triangleCentroid.x),
		triangleCentroid.y + scaleFactor * (normalEndPoint.y - triangleCentroid.y),
		triangleCentroid.z + scaleFactor * (normalEndPoint.z - triangleCentroid.z));

	DrawLine(triangleCentroid, scaleEndPoint, glm::vec3(0, 0, 1));

	return normalVector;
}

glm::vec3 Renderer::CalcNormal(const std::vector<glm::vec3>& vertexPositions)
{
	glm::vec3 p1 = glm::vec3((vertexPositions.at(0).x), (vertexPositions.at(0).y), (vertexPositions.at(0).z));
	glm::vec3 p2 = glm::vec3((vertexPositions.at(1).x), (vertexPositions.at(1).y), (vertexPositions.at(1).z));
	glm::vec3 p3 = glm::vec3((vertexPositions.at(2).x), (vertexPositions.at(2).y), (vertexPositions.at(2).z));

	glm::vec3 normalVector = glm::normalize(glm::cross(p2 - p1, p3 - p1));
	//normalVector = normalVector / glm::length(normalVector);

	return normalVector;
}

void Renderer::DrawBoundingBox(MeshModel& model, glm::mat4 transformation, const Scene& scene, Camera& cam)
{
	glm::mat4 orthoMat = cam.GetOrthographicTrans();
	glm::mat4 perspecMat = cam.GetPerspectiveTrans();
	glm::mat4 view = cam.GetCameraLookAt();

	glm::mat4 cameraLocalRotationMatX = glm::mat4(1, 0, 0, 0, 0, cos(cam.getSelfAngle().x), sin(cam.getSelfAngle().x), 0, 0, -sin(cam.getSelfAngle().x), cos(cam.getSelfAngle().x), 0, 0, 0, 0, 1);
	glm::mat4 cameraLocalRotationMatY = glm::mat4(cos(cam.getSelfAngle().y), 0, sin(cam.getSelfAngle().y), 0, 0, 1, 0, 0, -sin(cam.getSelfAngle().y), 0, cos(cam.getSelfAngle().y), 0, 0, 0, 0, 1);
	glm::mat4 cameraLocalRotationMatZ = glm::mat4(cos(cam.getSelfAngle().z), sin(cam.getSelfAngle().z), 0, 0, -sin(cam.getSelfAngle().z), cos(cam.getSelfAngle().z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	glm::mat4 cameraWorldRotationMatX = glm::mat4(1, 0, 0, 0, 0, cos(cam.getWorldRotatingAngle().x), sin(cam.getWorldRotatingAngle().x), 0, 0, -sin(cam.getWorldRotatingAngle().x), cos(cam.getWorldRotatingAngle().x), 0, 0, 0, 0, 1);
	glm::mat4 cameraWorldRotationMatY = glm::mat4(cos(cam.getWorldRotatingAngle().y), 0, sin(cam.getWorldRotatingAngle().y), 0, 0, 1, 0, 0, -sin(cam.getWorldRotatingAngle().y), 0, cos(cam.getWorldRotatingAngle().y), 0, 0, 0, 0, 1);
	glm::mat4 cameraWorldRotationMatZ = glm::mat4(cos(cam.getWorldRotatingAngle().z), sin(cam.getWorldRotatingAngle().z), 0, 0, -sin(cam.getWorldRotatingAngle().z), cos(cam.getWorldRotatingAngle().z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	glm::mat4 cameraLocalRotate = cameraLocalRotationMatX * cameraLocalRotationMatY * cameraLocalRotationMatZ;
	glm::mat4 cameraWorldRotate = cameraWorldRotationMatX * cameraWorldRotationMatY * cameraWorldRotationMatZ;

	cameraLocalRotate = glm::inverse(cameraLocalRotate);
	cameraWorldRotate = glm::inverse(cameraWorldRotate);

	transformation = orthoMat * perspecMat * cameraLocalRotate * view * cameraWorldRotate * transformation;

	int x0 = scene.GetWidth() / 2;
	int y0 = scene.GetHeight() / 2;
	int z0 = 500;

	glm::vec4 p1 = glm::vec4(maxPointXValue, maxPointYValue, maxPointZValue, 1);
	glm::vec4 p2 = glm::vec4(minPointXValue, maxPointYValue, maxPointZValue, 1);
	glm::vec4 p3 = glm::vec4(minPointXValue, minPointYValue, maxPointZValue, 1);
	glm::vec4 p4 = glm::vec4(maxPointXValue, minPointYValue, maxPointZValue, 1);

	glm::vec4 p5 = glm::vec4(maxPointXValue, maxPointYValue, minPointZValue, 1);
	glm::vec4 p6 = glm::vec4(minPointXValue, maxPointYValue, minPointZValue, 1);
	glm::vec4 p7 = glm::vec4(minPointXValue, minPointYValue, minPointZValue, 1);
	glm::vec4 p8 = glm::vec4(maxPointXValue, minPointYValue, minPointZValue, 1);

	p1 = transformation * p1;
	p2 = transformation * p2;
	p3 = transformation * p3;
	p4 = transformation * p4;
	p5 = transformation * p5;
	p6 = transformation * p6;
	p7 = transformation * p7;
	p8 = transformation * p8;

	glm::vec3 p1t = glm::vec3(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w);
	glm::vec3 p2t = glm::vec3(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w);
	glm::vec3 p3t = glm::vec3(p3.x / p3.w, p3.y / p3.w, p3.z / p3.w);
	glm::vec3 p4t = glm::vec3(p4.x / p4.w, p4.y / p4.w, p4.z / p4.w);
	glm::vec3 p5t = glm::vec3(p5.x / p5.w, p5.y / p5.w, p5.z / p5.w);
	glm::vec3 p6t = glm::vec3(p6.x / p6.w, p6.y / p6.w, p6.z / p6.w);
	glm::vec3 p7t = glm::vec3(p7.x / p7.w, p7.y / p7.w, p7.z / p7.w);
	glm::vec3 p8t = glm::vec3(p8.x / p8.w, p8.y / p8.w, p8.z / p8.w);

	p1t.x += x0; p1t.y += y0; p1t.z += z0;
	p2t.x += x0; p2t.y += y0; p2t.z += z0;
	p3t.x += x0; p3t.y += y0; p3t.z += z0;
	p4t.x += x0; p4t.y += y0; p4t.z += z0;
	p5t.x += x0; p5t.y += y0; p5t.z += z0;
	p6t.x += x0; p6t.y += y0; p6t.z += z0;
	p7t.x += x0; p7t.y += y0; p7t.z += z0;
	p8t.x += x0; p8t.y += y0; p8t.z += z0;

	if (scene.GetActiveModel().GetBoundingBoxShown())
	{
		DrawLine(p1t, p2t, glm::vec3(1, 0, 0));
		DrawLine(p2t, p3t, glm::vec3(1, 0, 0));
		DrawLine(p3t, p4t, glm::vec3(1, 0, 0));
		DrawLine(p4t, p1t, glm::vec3(1, 0, 0));

		DrawLine(p1t, p5t, glm::vec3(1, 0, 0));
		DrawLine(p2t, p6t, glm::vec3(1, 0, 0));
		DrawLine(p3t, p7t, glm::vec3(1, 0, 0));
		DrawLine(p4t, p8t, glm::vec3(1, 0, 0));

		DrawLine(p5t, p6t, glm::vec3(1, 0, 0));
		DrawLine(p6t, p7t, glm::vec3(1, 0, 0));
		DrawLine(p7t, p8t, glm::vec3(1, 0, 0));
		DrawLine(p8t, p5t, glm::vec3(1, 0, 0));
	}

	//scene.GetActiveModel().SetMinMax(glm::vec4(p1t.x, p2t.x, p1t.y, p3t.y)); // (maxX, minX, maxY, minY)
}