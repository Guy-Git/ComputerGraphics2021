#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "vector"
#include "Renderer.h"
#include "InitShader.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

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

	// (-1, 1)____(1, 1)
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

void Renderer::Render(const Scene& scene)
{
	int half_width = viewport_width_ / 2;
	int half_height = viewport_height_ / 2;
	int thickness = 15;

	if (scene.GetModelCount() > 0)
	{
		MeshModel currentModel = scene.GetActiveModel();

		double scaleFactor = abs(400 / FindMaxXorYPointForScaleFactor(currentModel));
		std::vector <glm::vec3> threePoints;
		for (int i = 0; i < currentModel.GetFacesCount(); i++)
		{
			threePoints.push_back(currentModel.GetVertex(currentModel.GetFace(i).GetVertexIndex(0) - 1));
			threePoints.push_back(currentModel.GetVertex(currentModel.GetFace(i).GetVertexIndex(1) - 1));
			threePoints.push_back(currentModel.GetVertex(currentModel.GetFace(i).GetVertexIndex(2) - 1));

			DrawTriangle(threePoints, scaleFactor * currentModel.GetScaleFactor(), currentModel.GetRotateAngle(), currentModel.GetPosition(),
				scene.GetScaleFactor(), scene.GetRotateAngle(), scene.GetPosition());

			DrawVertexNormals(threePoints);
			threePoints.clear();
		}
	}
	DrawLine(glm::ivec2(0, 500), glm::ivec2(1000, 500), glm::vec3(1, 1, 1)); // X axis

	DrawLine(glm::ivec2(500, 1000), glm::ivec2(500, 0), glm::vec3(1, 1, 1)); // Y axis

	/*DrawLine(glm::ivec2(400, 400), glm::ivec2(410, 500), glm::vec3(1, 0, 1)); // MEGENTA +10

	DrawLine(glm::ivec2(400, 400), glm::ivec2(600, 200), glm::vec3(1, 1, 1)); // WHITE -1

	DrawLine(glm::ivec2(400, 400), glm::ivec2(600, 300), glm::vec3(0, 0, 0)); // BLACK -0.5

	DrawLine(glm::ivec2(500, 200), glm::ivec2(400, 400), glm::vec3(0, 1, 1)); // CYAN -2
	*/

	/*for (int i = 0; i < 200; i++)
	{
		DrawLine(glm::ivec2(0, 0), glm::ivec2(100 + r + 100 * (sin((2 * PI * i) / a)), 100 + r - 100 * (cos((2 * PI * i) / a))), glm::vec3(1, 1, 0));
	}

	for (int i = 0; i < 60; i++)
	{
		if (i < 5) // horns
		{
			DrawLine(glm::ivec2(440 + i, 520 - i), glm::ivec2(480, 520 - i), glm::vec3(0, 0, 0));
			DrawLine(glm::ivec2(493, 520 - i), glm::ivec2(498, 520 - i), glm::vec3(0, 0, 0));
			DrawLine(glm::ivec2(502, 520 - i), glm::ivec2(507, 520 - i), glm::vec3(0, 0, 0));
			DrawLine(glm::ivec2(520, 520 - i), glm::ivec2(560 - i, 520 - i), glm::vec3(0, 0, 0));
		}
		else if (i >= 5 && i < 15) { // head
			DrawLine(glm::ivec2(440 + i, 520 - i), glm::ivec2(480, 520 - i), glm::vec3(0, 0, 0));
			DrawLine(glm::ivec2(492, 520 - i), glm::ivec2(508, 520 - i), glm::vec3(0, 0, 0));
			DrawLine(glm::ivec2(520, 520 - i), glm::ivec2(560 - i, 520 - i), glm::vec3(0, 0, 0));
		}
		else { // body
			DrawLine(glm::ivec2(440 + i, 520 - i), glm::ivec2(560 - i, 520 - i), glm::vec3(0, 0, 0));
		}

		if (i > 30 && i < 42)
		{
			DrawLine(glm::ivec2(470, 520 - i), glm::ivec2(530, 520 - i), glm::vec3(0, 0, 0));
		}
	}
	// sanity check
	for (int i = 0; i < 200; i++)
	{
		DrawLine(glm::ivec2(x0, y0), glm::ivec2(x0 + r * (sin((2 * PI * i) / a)), y0 + r * (cos((2 * PI * i) / a))), glm::vec3(0, 1, 1));
	}
	*/
}

double Renderer::FindMaxXorYPointForScaleFactor(MeshModel& currentModel)
{
	// find the max vertex point value to set the scale factor
	double maxPointXValue = -INFINITY;
	double maxPointYValue = -INFINITY;
	for (int i = 0; i < currentModel.GetVerticesCount(); i++)
	{
		if (currentModel.GetVertex(i).x > maxPointXValue)
		{
			maxPointXValue = currentModel.GetVertex(i).x;
		}
		if (currentModel.GetVertex(i).y > maxPointYValue)
		{
			maxPointYValue = currentModel.GetVertex(i).y;
		}
	}

	return maxPointXValue > maxPointYValue ? maxPointXValue : maxPointYValue;
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

void Renderer::DrawTriangle(const std::vector<glm::vec3>& vertexPositions, float localScale, glm::vec3 localRotAngle, glm::vec3 localPosition,
	float worldScale, glm::vec3 worldRotAngle, glm::vec3 worldPosition)
{
	int x0 = 500;
	int y0 = 500;

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

	glm::vec4 p1 = glm::vec4((vertexPositions.at(0).x), (vertexPositions.at(0).y), (vertexPositions.at(0).z), 1);
	glm::vec4 p2 = glm::vec4((vertexPositions.at(1).x), (vertexPositions.at(1).y), (vertexPositions.at(1).z), 1);
	glm::vec4 p3 = glm::vec4((vertexPositions.at(2).x), (vertexPositions.at(2).y), (vertexPositions.at(2).z), 1);

	glm::mat4 transformation = worldScaleMat * worldRotationMatX * worldRotationMatY * worldRotationMatZ * worldPositionMat *
		localScaleMat * localRotationMatX * localRotationMatY * localRotationMatZ * localPositionMat;

	p1 = transformation * p1;
	p2 = transformation * p2;
	p3 = transformation * p3;

	glm::vec2 p1t = glm::vec2(p1.x, p1.y);
	glm::vec2 p2t = glm::vec2(p2.x, p2.y);
	glm::vec2 p3t = glm::vec2(p3.x, p3.y);

	p1t.x += x0;
	p1t.y += y0;
	p2t.x += x0;
	p2t.y += y0;
	p3t.x += x0;
	p3t.y += y0;

	DrawLine(p1t, p2t, glm::vec3(0, 0, 0));
	DrawLine(p2t, p3t, glm::vec3(0, 0, 0));
	DrawLine(p1t, p3t, glm::vec3(0, 0, 0));
}

void Renderer::DrawVertexNormals(const std::vector<glm::vec3>& vertexPositions)
{
	glm::vec3 p1 = glm::vec3((vertexPositions.at(0).x), (vertexPositions.at(0).y), 0);
	glm::vec3 p2 = glm::vec3((vertexPositions.at(1).x), (vertexPositions.at(1).y), 0);
	glm::vec3 p3 = glm::vec3((vertexPositions.at(2).x), (vertexPositions.at(2).y), 0);

	glm::vec3 directionOfNorm = glm::cross((p2 - p1), (p3 - p1));
	glm::vec3 norm = directionOfNorm / glm::length(directionOfNorm);

	glm::vec2 norm2D = glm::vec2(norm.x, norm.y);
	glm::vec2 p12D = glm::vec2(p1.x, p1.y);

}
