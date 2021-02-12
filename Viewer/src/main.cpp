#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>

#include <nfd.h>

#include <stdio.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
//#include "ImguiMenus.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "Utils.h"

float zoomFactor = 1;
int windowWidth = 1280;
int windowHeight = 720;
char* windowTitle = "Guy & Yuval";
glm::vec4 clearColor = glm::vec4(0.26f, 0.26f, 0.26f, 1);
bool zoomChanged = false;
std::shared_ptr<Scene> scene;

ImGuiIO* imgui;
GLFWwindow* window;

bool light_properties_window = false;
int light_type_selection = 0;
glm::vec3 light_transformation = glm::vec3(0);
glm::vec3 light_direction = glm::vec3(0);
glm::vec3 model_color = glm::vec3(0.156, 0.592, 0.976);
glm::vec3 light_color = glm::vec3(1);

static int model_selection = 0;
static int light_selection = 0;
static int pointOrParallelLight = -1;
static int camera_selection = 0;
static int view_selection = 0;
static int last_model_selection = 0;

bool show_global_rotation_window = false;
bool show_global_scale_window = false;
bool show_global_translation_window = false;
bool camera_global_rotation_window = false;
bool light_translation_window = false;
bool show_local_rotation_window = false;
bool show_local_scale_window = false;
bool show_local_translation_window = false;

static float scale_factor_local = 1.0;
static glm::vec3 rotation_angle_local = glm::vec3(0);
static glm::vec3 transformation_local = glm::vec3(0);

static glm::vec3 camera_rotation_angle_local = glm::vec3(0);
static glm::vec3 camera_transformation_local = glm::vec3(0);
static glm::vec3 camera_rotation_world = glm::vec3(0);

static float scale_factor_global = 1.0;
static glm::vec3 rotation_angle_global = glm::vec3(0);
static glm::vec3 transformation_global = glm::vec3(0);

bool show_model_1_window = false;
bool show_model_2_window = false;
bool show_model_3_window = false;
bool show_model_4_window = false;

float fovy = 1;
float zNear = 1;
float zFar = 1;
float height = 1;

bool model_materials_window = false;

bool vertex_normal = false;
bool face_normal = false;
bool show_bounding_box = false;

bool rotate_light = false;

bool show_warning_window = false;
bool show_model_selection_window = false;

glm::vec3 cameraAtBuffer = glm::vec3(0.0, 0.0, 0.0);

static float cameraX = 0;
static float cameraY = 0;
static float cameraZ = 10.0;

static float orthoZoom = 0.0;

float lastX = 750, lastY = 450;
bool firstMouse = true;
float yaw = 0;
float pitch = 0;

float ambient = 0.5f;
float diffuse = 0.5f;
float specular = 0.5f;

bool texture_mapping = 0;

bool camera_local_rotation_window = false;
bool camera_local_translation_window = false;

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupImgui(GLFWwindow* window);
bool Setup(int windowWidth, int windowHeight, const char* windowName);
void Cleanup();

static void GlfwErrorCallback(int error, const char* description);
void RenderFrame(GLFWwindow* window, std::shared_ptr<Scene> scene, Renderer& renderer, ImGuiIO& io);

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height);

float GetAspectRatio();
void DrawImguiMenus();
void HandleImguiInput();
void ChangeCameraSelection(const std::shared_ptr<Scene>& scene);
void ChangeLightSelection(const std::shared_ptr<Scene>& scene);
void SwitchToDifferentModelView(int modelID);
void ResetParametersValue(const std::shared_ptr<Scene>& scene);
void ResetLightParametersValue(const std::shared_ptr<Scene>& scene);
void SetParametersValueChangingModels(const std::shared_ptr<Scene>& scene);
void SetParametersValueChangingLights(const std::shared_ptr<Scene>& scene);
void ShowScaleRotateTranslationWindowsLocal(const std::shared_ptr<Scene>& scene);
void ShowScaleRotateTranslationWindowsGlobal(const std::shared_ptr<Scene>& scene);
bool IsPositionInBoundingBox(float xPos, float yPos, Scene& scene);
void CameraWindowsLocal(const std::shared_ptr<Scene>& scene);
void CameraWindowsWorld(const std::shared_ptr<Scene>& scene);
void LightWindow(Scene& scene);

int main(int argc, char** argv)
{

	if (!Setup(windowWidth, windowHeight, windowTitle))
	{
		std::cerr << "Setup failed" << std::endl;
		return -1;
	}

	scene = std::make_shared<Scene>();
	glm::vec3 eye = glm::vec3(0, 0, 10);
	glm::vec3 at = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	Camera camera1 = Camera(eye, at, up, GetAspectRatio());
	Camera camera2 = Camera(eye, at, up, GetAspectRatio());
	scene->AddCamera(camera1);
	scene->AddCamera(camera2);

	scene->AddLight(std::make_shared<PointLight>(glm::vec3(0, 0, 15), glm::vec3(1, 1, 1)));
	scene->AddLight(std::make_shared<PointLight>(glm::vec3(0, 5, 5), glm::vec3(0, 0, 0)));
	scene->AddLight(std::make_shared<PointLight>(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0)));

	Renderer renderer;
	renderer.LoadShaders();
	renderer.LoadTextures();

	while (!glfwWindowShouldClose(window))
	{
		// Poll and process events
		glfwPollEvents();

		// Imgui stuff
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawImguiMenus();
		ImGui::Render();
		HandleImguiInput();

		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene
		renderer.Render(scene);

		// Imgui stuff
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool Setup(int windowWidth, int windowHeight, const char* windowName)
{
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, windowName);
	if (!window)
	{
		std::cerr << "Window setup failed" << std::endl;
		return false;
	}

	imgui = &SetupImgui(window);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glEnable(GL_DEPTH_TEST);

	return true;
}

GLFWwindow* SetupGlfwWindow(int windowWidth, int windowHeight, const char* windowName)
{
	// Intialize GLFW
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create an OpenGL 3.3 core, forward compatible context window
	window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(window);

	// Setup window events callbacks
	glfwSetFramebufferSizeCallback(window, glfw_OnFramebufferSize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// An error occured
		std::cerr << "GLAD initialization failed" << std::endl;
		return false;
	}

	return window;
}

ImGuiIO& SetupImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();

	glfwSetScrollCallback(window, glfw_OnMouseScroll);

	return io;
}

void HandleImguiInput()
{
	if (!imgui->WantCaptureKeyboard)
	{
		if (imgui->KeysDown[49]) // 1
		{
			scene->GetActiveModel()->RotateXModel(M_PI / 200);
		}

		if (imgui->KeysDown[50]) // 2
		{
			scene->GetActiveModel()->RotateXModel(-M_PI / 200);
		}

		if (imgui->KeysDown[51]) // 3
		{
			scene->GetActiveModel()->RotateYModel(M_PI / 200);
		}

		if (imgui->KeysDown[52]) // 4
		{
			scene->GetActiveModel()->RotateYModel(-M_PI / 200);
		}

		if (imgui->KeysDown[53]) // 5
		{
			scene->GetActiveModel()->RotateZModel(M_PI / 200);
		}

		if (imgui->KeysDown[54]) // 6
		{
			scene->GetActiveModel()->RotateZModel(-M_PI / 200);
		}

		if (imgui->KeysDown[45]) // -
		{
			scene->GetActiveModel()->ScaleModel(1 / 1.01);
		}

		if (imgui->KeysDown[61]) // +
		{
			scene->GetActiveModel()->ScaleModel(1.01);
		}

		if (imgui->KeysDown[81]) // q
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0.02, 0));
		}

		if (imgui->KeysDown[69]) // e
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, -0.02, 0));
		}

		if (imgui->KeysDown[65]) // a
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(-0.02, 0, 0));
		}

		if (imgui->KeysDown[68]) // d
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0.02, 0, 0));
		}

		if (imgui->KeysDown[83]) // s
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, 0.02));
		}

		if (imgui->KeysDown[87]) // w
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, -0.02));
		}
	}

	if (!imgui->WantCaptureMouse)
	{
		if (zoomChanged)
		{
			scene->GetActiveCamera().Zoom(zoomFactor);
			zoomChanged = false;
		}
	}
}

void Cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
	scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
}

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	zoomFactor = glm::pow(1.1, -yoffset);
	zoomChanged = true;
}

float GetAspectRatio()
{
	return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}

void DrawImguiMenus()
{
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
	{
		ImGui::Begin("Scene Menu");
		if (ImGui::ColorEdit3("Clear Color", (float*)&clearColor))
		{
			glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		}

		if (ImGui::CollapsingHeader("Cameras"))
		{
			if (ImGui::Button("Add Camera"))
			{

				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_real_distribution<double> dist(0, 2 * M_PI);
				std::uniform_real_distribution<double> dist2(2, 10);
				double angle = dist(mt);
				double radius = dist2(mt);

				glm::vec4 eye = glm::vec4(radius * glm::cos(angle), 0, radius * glm::sin(angle), 1);
				glm::vec4 at = glm::vec4(0, 0, 0, 1);
				glm::vec4 up = glm::vec4(0, 1, 0, 1);
				scene->AddCamera(Camera(-scene->GetActiveCamera().GetEye(), at, up, GetAspectRatio()));
			}

			const char** items;
			std::vector<std::string> cameraStrings;
			items = new const char* [scene->GetCameraCount()];
			for (int i = 0; i < scene->GetCameraCount(); i++)
			{
				std::ostringstream s;
				s << "Camera " << i;
				std::string mystring = s.str();
				cameraStrings.push_back(mystring);
			}

			for (int i = 0; i < scene->GetCameraCount(); i++)
			{
				items[i] = cameraStrings[i].c_str();
			}

			int currentCamera = scene->GetActiveCameraIndex();
			ImGui::Combo("Active Camera", &currentCamera, items, scene->GetCameraCount());

			if (currentCamera != scene->GetActiveCameraIndex())
			{
				scene->SetActiveCameraIndex(currentCamera);
				scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
			}

			delete items;

			int newProjectionType = scene->GetActiveCamera().IsPrespective() ? 0 : 1;
			ImGui::RadioButton("Prespective", &newProjectionType, 0);
			ImGui::RadioButton("Orthographic", &newProjectionType, 1);

			if (newProjectionType == 0)
			{
				float fovy;
				float zNear;
				float zFar;

				scene->GetActiveCamera().SwitchToPrespective();

				if (ImGui::SliderFloat("Fovy", &fovy, 0.0f, M_PI))
				{
					scene->GetActiveCamera().SetFovy(fovy);
				}

				if (ImGui::SliderFloat("Near", &zNear, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetNear(zNear);
				}

				if (ImGui::SliderFloat("Far", &zFar, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetFar(zFar);
				}
			}
			else if (newProjectionType == 1)
			{
				float height;
				float zNear;
				float zFar;

				scene->GetActiveCamera().SwitchToOrthographic();

				if (ImGui::SliderFloat("Height", &height, 0.0f, M_PI))
				{
					scene->GetActiveCamera().SetHeight(height);
				}

				if (ImGui::SliderFloat("Near", &zNear, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetNear(zNear);
				}

				if (ImGui::SliderFloat("Far", &zFar, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetFar(zFar);
				}
			}
		}

		if (ImGui::CollapsingHeader("Models"))
		{
			const char** items;
			std::vector<std::string> modelStrings;
			items = new const char* [scene->GetModelCount()];
			for (int i = 0; i < scene->GetModelCount(); i++)
			{
				std::ostringstream s;
				s << scene->GetModel(i)->GetModelName();
				std::string mystring = s.str();
				modelStrings.push_back(mystring);
			}

			for (int i = 0; i < scene->GetModelCount(); i++)
			{
				items[i] = modelStrings[i].c_str();
			}

			int currentModelIndex = scene->GetActiveModelIndex();
			ImGui::Combo("Active Model", &currentModelIndex, items, scene->GetModelCount());

			if (currentModelIndex != scene->GetActiveModelIndex())
			{
				scene->SetActiveModelIndex(currentModelIndex);
			}

			delete items;

			glm::vec3 modelColor = scene->GetActiveModel()->GetColor();
			if (ImGui::ColorEdit3("Model Color", (float*)&modelColor))
			{
				scene->GetActiveModel()->SetColor(modelColor);
			}

			std::shared_ptr<MeshModel> meshModel = std::dynamic_pointer_cast<MeshModel>(scene->GetActiveModel());
			if (meshModel)
			{
				//glm::vec4 normalColor = meshModel->GetNormalModel().GetColor();
				//if (ImGui::ColorEdit3("Normal Color", (float*)&normalColor))
				//{
				//	meshModel->GetNormalModel().SetColor(normalColor);
				//}
			}
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
					nfdchar_t* outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						scene->AddModel(Utils::LoadMeshModel(outPath));
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				ImGui::EndMenu();

			}
			//ImGui::EndMainMenuBar();
		}

	}
	{
		if (ImGui::BeginMenu("Model Local Trans."))
		{
			if (ImGui::MenuItem("Translation"))
			{
				show_local_translation_window = true;
			}

			if (ImGui::MenuItem("Rotation"))
			{
				show_local_rotation_window = true;
			}

			if (ImGui::MenuItem("Scale"))
			{
				show_local_scale_window = true;
			}

			if (ImGui::MenuItem("Materials"))
			{
				model_materials_window = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Model World Trans."))
		{
			if (ImGui::MenuItem("Translation"))
			{
				show_global_translation_window = true;
			}

			if (ImGui::MenuItem("Rotation"))
			{
				show_global_rotation_window = true;
			}

			if (ImGui::MenuItem("Scale"))
			{
				show_global_scale_window = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera Local Trans."))
		{
			if (ImGui::MenuItem("Rotate"))
			{
				camera_local_rotation_window = true;
			}

			if (ImGui::MenuItem("Translate"))
			{
				camera_local_translation_window = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera World Trans."))
		{
			if (ImGui::MenuItem("Rotate"))
			{
				camera_global_rotation_window = true;
			}

			ImGui::EndMenu();
		}

		/*if (ImGui::BeginMenu("Light"))
		{
			light_properties_window = true;
			ImGui::EndMenu();
		}*/

		if (ImGui::BeginMenu("Post Processing"))
		{
			if (ImGui::MenuItem("Gaussing Blur + Bloom"))
			{
				//scene.SetPPMode(1);
			}

			if (ImGui::MenuItem("Fog"))
			{
				//scene.SetPPMode(2);
			}
			if (ImGui::MenuItem("Reset PP"))
			{
				//scene.SetPPMode(0);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// camera selection window - position and window flag
	ImGui::SetNextWindowPos(ImVec2(0, windowHeight - 200));
	ImGui::SetNextWindowSize(ImVec2(330, 200));

	ImGui::Begin("Camera selection", &show_model_selection_window, ImGuiWindowFlags_NoMove);

	ImGui::RadioButton("Camera #1", &camera_selection, 0);
	ImGui::RadioButton("Camera #2", &camera_selection, 1);

	if (ImGui::Button("Reset Current Camera"))
	{
		view_selection = -1;
		//scene.GetActiveCamera().ResetProjectionsMatrix();
		//scene.GetActiveCamera().ResetPerspectiveTrans();
	}

	int newProjectionType = scene->GetActiveCamera().IsPrespective() ? 0 : 1;
	ImGui::RadioButton("Prespective", &newProjectionType, 0);
	ImGui::RadioButton("Orthographic", &newProjectionType, 1);

	if (newProjectionType == 0)
	{


		scene->GetActiveCamera().SwitchToPrespective();

		if (ImGui::SliderFloat("Fovy", &fovy, 0.0f, 10.0))
		{
			scene->GetActiveCamera().SetFovy(fovy);
		}

		if (ImGui::SliderFloat("Near", &zNear, 1.0f, 10.0f))
		{
			scene->GetActiveCamera().SetNear(zNear);
		}

		if (ImGui::SliderFloat("Far", &zFar, 1.0f, 10.0f))
		{
			scene->GetActiveCamera().SetFar(zFar);
		}
	}
	else if (newProjectionType == 1)
	{
		scene->GetActiveCamera().SwitchToOrthographic();

		if (ImGui::SliderFloat("Height", &height, 0.0f, 10.0))
		{
			scene->GetActiveCamera().SetHeight(height);
		}

		if (ImGui::SliderFloat("Near", &zNear, 1.0f, 10.0f))
		{
			scene->GetActiveCamera().SetNear(zNear);
		}

		if (ImGui::SliderFloat("Far", &zFar, 1.0f, 10.0f))
		{
			scene->GetActiveCamera().SetFar(zFar);
		}
	}

	if (ImGui::Button("Reset Camera Position"))
	{
		//scene.GetActiveCamera().ResetCameraPosition();
	}

	ImGui::End();

	if (camera_selection != scene->GetActiveCameraIndex())
	{
		scene->SetActiveCameraIndex(camera_selection);
		scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
	}

	//switch (view_selection)
	//{
	//case 0:
	//	//scene.GetActiveCamera().ResetProjectionsMatrix();

	//	//scene.GetActiveCamera().SetOrthographicTrans(left, right + orthoZoom, bottom, top + orthoZoom, near_param, far_param);

	//	//scene.GetActiveCamera().SetCameraLookAt(scene.GetActiveCamera().GetCameraEye(),
	//		//scene.GetActiveCamera().GetCameraAt(),
	//		//scene.GetActiveCamera().GetCameraUp());

	//	break;
	//case 1:
	//	//scene.GetActiveCamera().ResetProjectionsMatrix();
	//	//.GetActiveCamera().SetPerspectiveTrans(glm::radians(fov), 1, near_param, far_param);
	//	//scene.GetActiveCamera().SetCameraLookAt(scene.GetActiveCamera().GetCameraEye(),
	//		//scene.GetActiveCamera().GetCameraAt(),
	//		//scene.GetActiveCamera().GetCameraUp());

	//	break;
	//}

	// model selection window - position and window flag
	ImGui::SetNextWindowPos(ImVec2(330, windowHeight - 200));
	ImGui::SetNextWindowSize(ImVec2(330, 200));

	ImGui::Begin("Model selection", &show_model_selection_window, ImGuiWindowFlags_NoMove);

	ImGui::RadioButton("Model #1", &model_selection, 0);
	if (scene->GetModelCount() > 0)
	{
		ImGui::SameLine(); ImGui::Text("[MODEL LOADED - %s]", scene->GetModel(0)->GetModelName().c_str());
	}
	ImGui::RadioButton("Model #2", &model_selection, 1);
	if (scene->GetModelCount() > 1)
	{
		ImGui::SameLine(); ImGui::Text("[MODEL LOADED - %s]", scene->GetModel(1)->GetModelName().c_str());
	}
	if (ImGui::Button("Reset Current Model"))
	{
		ResetParametersValue(scene);
	}
	ImGui::Checkbox("Show Vertex Normals", &vertex_normal);
	ImGui::SameLine(); ImGui::Checkbox("Show Face Normals", &face_normal);
	ImGui::Checkbox("Show Bounding Box", &show_bounding_box);

	ImGui::End();

	// light selection window - position and window flag
	ImGui::SetNextWindowPos(ImVec2(660, windowHeight - 200));
	ImGui::SetNextWindowSize(ImVec2(420, 200));

	ImGui::Begin("Light selection", &show_model_selection_window, ImGuiWindowFlags_NoMove);

	//ChangeLightSelection(scene);

	if (ImGui::RadioButton("Point Of Light", &pointOrParallelLight, 0))
	{

		//scene->AddLight(Utils::LoadMeshModel("../Data/cube.obj"));

		light_selection = scene->GetLightCount() - 1;

		//scene.GetActiveLight().SetColorOfMesh(glm::vec3(1, 1, 1));

		//scene.GetActiveLight().kindOfModel = 1; // point
	}

	if (ImGui::RadioButton("Parallel Light", &pointOrParallelLight, 1))
	{
		//scene.RemoveLight();

		std::vector<Face> faces;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::string string;

		//scene.AddLight(std::make_shared<MeshModel>(faces, vertices, normals, string));
		//scene.SetActiveLightIndex(0);
		//scene.GetActiveLight().SetColorOfMesh(glm::vec3(1, 1, 1));
		//scene.GetActiveLight().kindOfModel = 2; // parallel
	}

	if (pointOrParallelLight != -1)
	{
		ImGui::RadioButton("Flat", &light_type_selection, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Phong", &light_type_selection, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Gouraud", &light_type_selection, 2);

		// Controls
		ImGui::ColorEdit3("Light Color", (float*)&light_color);

		if (pointOrParallelLight == 0)
		{
			ImGui::SliderFloat("Light X Position", &light_transformation.x, -50.0, 50.0);
			ImGui::SliderFloat("Light Y Position", &light_transformation.y, -25.0, 25.0);
			ImGui::SliderFloat("Light Z Position", &light_transformation.z, -100.0, 100.0);
		}

		else
		{
			ImGui::SliderFloat("Light X Direction", &light_direction.x, -1, 1);
			ImGui::SliderFloat("Light Y Direction", &light_direction.y, -1, 1);
			ImGui::SliderFloat("Light Z Direction", &light_direction.z, -1, 1);
		}

		if (scene->GetLightCount() > 0)
		{
			//scene.GetActiveLight().SetLightModel(light_type_selection);
			//scene.GetActiveLight().SetColorOfMesh(light_color);
			//scene.GetActiveLight().SetNewPosition(light_transformation);
			//scene.GetActiveLight().SetNewLightDirection(light_direction);
		}
	}

	if (ImGui::Button("Reset Current light"))
	{
		//ResetLightParametersValue(scene);
	}

	ImGui::End();

	ShowScaleRotateTranslationWindowsLocal(scene); // all local windows declarations

	ShowScaleRotateTranslationWindowsGlobal(scene); // all global windows declarations

	SwitchToDifferentModelView(model_selection);

	CameraWindowsLocal(scene);

	CameraWindowsWorld(scene);

	//LightWindow(scene);

	//SetNormalsAndBoundingBox(scene);

	if (show_model_1_window)
	{
		if (scene->GetModelCount() > 0)
			scene->SetActiveModelIndex(model_selection);
	}

	if (show_model_2_window)
	{
		if (scene->GetModelCount() > 1)
			scene->SetActiveModelIndex(model_selection);
	}

	//SetParametersValueChangingModels(scene);
	SetParametersValueChangingLights(scene);
}

void ChangeLightSelection(Scene& scene)
{
	if (light_selection == 0)
	{
		//scene.SetActiveLightIndex(0);
	}
	if (light_selection == 1)
	{
		//scene.SetActiveLightIndex(1);
	}
}

void SwitchToDifferentModelView(int modelID) {
	switch (modelID)
	{
	case 0:
		show_model_1_window = true;
		show_model_2_window = false;
		break;
	case 1:
		show_model_1_window = false;
		show_model_2_window = true;
		break;
	default:
		break;
	}
}

//void SetParametersValueChangingModels(const std::shared_ptr<Scene>& scene)
//{
//	if (scene->GetModelCount() > 0)
//	{
//		//scale_factor_local = scene->GetActiveModel().GetScaleFactor();
//		//rotation_angle_local = scene->GetActiveModel().GetRotateAngle();
//		//transformation_local = scene->GetActiveModel().GetPosition();
//
//		//scale_factor_global = scene->GetScaleFactor();
//		//rotation_angle_global = scene->GetRotateAngle();
//		//transformation_global = scene->GetPosition();
//	}
//}

void SetParametersValueChangingLights(const std::shared_ptr<Scene>& scene)
{
	if (scene->GetLightCount() > 0)
	{
		//light_transformation = scene.GetActiveLight().GetPosition();
	}
}

void ResetParametersValue(const std::shared_ptr<Scene>& scene)
{
	if (scene->GetModelCount() > 0)
	{
		vertex_normal = false;
		face_normal = false;
		show_bounding_box = false;

		scene->GetActiveModel()->ScaleModel(0);
		scene->GetActiveModel()->RotateXModel(0);
		scene->GetActiveModel()->RotateYModel(0);
		scene->GetActiveModel()->RotateZModel(0);
		scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, 0));

		scale_factor_local = 1;
		rotation_angle_local = glm::vec3(0);;
		transformation_local = glm::vec3(0);

		scale_factor_global = 1;
		rotation_angle_global = glm::vec3(0);;
		transformation_global = glm::vec3(0);


		scene->GetActiveModel()->ScaleWorld(0);
		scene->GetActiveModel()->RotateXWorld(0);
		scene->GetActiveModel()->RotateYWorld(0);
		scene->GetActiveModel()->RotateZWorld(0);
		scene->GetActiveModel()->TranslateWorld(glm::vec3(0, 0, 0));
	}
}

void ResetLightParametersValue(const std::shared_ptr<Scene>& scene)
{
	if (scene->GetLightCount() > 0)
	{
		light_transformation = glm::vec3(0);
		//scene.GetActiveLight().SetNewPosition(light_transformation);
	}
}

void ShowScaleRotateTranslationWindowsLocal(const std::shared_ptr<Scene>& scene) {
	if (show_local_rotation_window)
	{
		ImGui::Begin("Local Rotation Window", &show_local_rotation_window);
		ImGui::Text("Choose axis and start rotate");

		if (ImGui::Button("Rotate Left X"))
		{
			scene->GetActiveModel()->RotateXModel(0.392699082);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right X")) {
			scene->GetActiveModel()->RotateXModel(-0.392699082);
		}

		if (ImGui::Button("Rotate Left Y"))
		{
			scene->GetActiveModel()->RotateYModel(0.392699082);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Y")) {
			scene->GetActiveModel()->RotateYModel(-0.392699082);
		}
		if (ImGui::Button("Rotate Left Z"))
		{
			scene->GetActiveModel()->RotateZModel(0.392699082);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Z")) {
			scene->GetActiveModel()->RotateZModel(-0.392699082);
		}

		if (ImGui::Button("Close Me")) {
			show_local_rotation_window = false;
		}
		ImGui::End();
	}

	if (show_local_scale_window)
	{
		ImGui::Begin("Local Scale Window", &show_local_scale_window, ImGuiWindowFlags_None);
		ImGui::Text("Scale Model");

		if (ImGui::Button("+"))
		{
			scene->GetActiveModel()->ScaleModel(2.01);
		}

		ImGui::SameLine();

		if (ImGui::Button("-")) {
			scene->GetActiveModel()->ScaleModel(1 / 2.01);
		}

		if (ImGui::Button("Close Me")) {
			show_local_scale_window = false;
		}

		ImGui::End();
	}

	if (show_local_translation_window)
	{
		ImGui::Begin("Local Translation Window", &show_local_translation_window);

		if (ImGui::Button("Move X positive"))
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0.5, 0, 0));
		}

		ImGui::SameLine();

		if (ImGui::Button("Move X negetive")) {
			scene->GetActiveModel()->TranslateModel(glm::vec3(-0.5, 0, 0));
		}

		if (ImGui::Button("Move Y positive"))
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0.5, 0));
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Y negetive")) {
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, -0.5, 0));
		}

		if (ImGui::Button("Move Z positive"))
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, 0.5));
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Z negetive")) {
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, -0.5));
		}

		if (ImGui::Button("Close Me")) {
			show_local_translation_window = false;
		}

		ImGui::End();
	}

	if (model_materials_window)
	{
		ImGui::Begin("Model Material Window", &model_materials_window);

		ImGui::Text("Choose a Material: ");
		ImGui::SameLine();
		if (ImGui::Button("Metal"))
		{
			ambient = 0.2;
			diffuse = 0.9;
			specular = 1.0;
			model_color = glm::vec3(174, 174, 174);
		}

		ImGui::SameLine();
		if (ImGui::Button("Plastic"))
		{
			ambient = 0.4;
			diffuse = 0.5;
			specular = 0.4;
			model_color = glm::vec3(238, 33, 33);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rubber"))
		{
			ambient = 0.3;
			diffuse = 0.7;
			specular = 0.2;
			model_color = glm::vec3(63, 69, 113);
		}

		ImGui::ColorEdit3("Model Color", (float*)&model_color);

		ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse", &diffuse, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f);

		if (scene->GetModelCount() > 0)
		{
			/*scene.GetActiveModel().SetAmbient(ambient);
			scene.GetActiveModel().SetDiffuse(diffuse);
			scene.GetActiveModel().SetSpecular(specular);
			scene.GetActiveModel().SetColorOfMesh(glm::normalize(model_color));*/
		}

		ImGui::End();
	}
}

void ShowScaleRotateTranslationWindowsGlobal(const std::shared_ptr<Scene>& scene) {
	if (show_global_rotation_window)
	{
		ImGui::Begin("Global Rotation Window", &show_global_rotation_window);
		ImGui::Text("Rotation left or right");

		if (ImGui::Button("Rotate Left X"))
		{
			scene->GetActiveModel()->RotateXWorld(0.392699082);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right X")) {
			scene->GetActiveModel()->RotateXWorld(-0.392699082);
		}

		if (ImGui::Button("Rotate Left Y"))
		{
			scene->GetActiveModel()->RotateYWorld(0.392699082);
		}

		ImGui::SameLine();

		if (ImGui::Button("Rotate Right Y")) {
			scene->GetActiveModel()->RotateYWorld(-0.392699082);
		}
		if (ImGui::Button("Rotate Left Z"))
		{
			scene->GetActiveModel()->RotateZWorld(0.392699082);
		}

		ImGui::SameLine();

		if (ImGui::Button("Rotate Right Z")) {
			scene->GetActiveModel()->RotateZWorld(-0.392699082);
		}

		if (ImGui::Button("Close Me")) {
			show_global_rotation_window = false;
		}
		ImGui::End();
	}

	if (show_global_scale_window)
	{
		ImGui::Begin("Global Scale Window", &show_global_scale_window);
		ImGui::Text("Scale Model");

		if (ImGui::Button("+"))
		{
			scene->GetActiveModel()->ScaleWorld(2.01);
		}

		ImGui::SameLine();

		if (ImGui::Button("-")) {
			scene->GetActiveModel()->ScaleWorld(1 / 2.01);
		}

		if (ImGui::Button("Close Me")) {
			show_global_scale_window = false;
		}

		ImGui::End();
	}

	if (show_global_translation_window)
	{
		ImGui::Begin("Global Translation Window", &show_global_translation_window);

		if (ImGui::Button("Move X positive"))
		{
			scene->GetActiveModel()->TranslateWorld(glm::vec3(0.5, 0, 0));
		}

		ImGui::SameLine();

		if (ImGui::Button("Move X negetive")) {
			scene->GetActiveModel()->TranslateWorld(glm::vec3(-0.5, 0, 0));
		}

		if (ImGui::Button("Move Y positive"))
		{
			scene->GetActiveModel()->TranslateWorld(glm::vec3(0, 0.5, 0));
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Y negetive")) {
			scene->GetActiveModel()->TranslateWorld(glm::vec3(0, -0.5, 0));
		}

		if (ImGui::Button("Move Z positive"))
		{
			scene->GetActiveModel()->TranslateWorld(glm::vec3(0, 0, 0.5));
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Z negetive")) {
			scene->GetActiveModel()->TranslateWorld(glm::vec3(0, 0, -0.5));
		}

		if (ImGui::Button("Close Me")) {
			show_global_translation_window = false;
		}

		ImGui::End();
	}
}

void CameraWindowsLocal(const std::shared_ptr<Scene>& scene)
{
	if (camera_local_rotation_window)
	{
		ImGui::Begin("Camera Local Rotation", &camera_local_rotation_window);

		if (ImGui::Button("Rotate Left X"))
		{
			camera_rotation_angle_local.x += 0.392699082;
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right X"))
		{
			camera_rotation_angle_local.x -= 0.392699082;
		}

		if (ImGui::Button("Rotate Left Y"))
		{
			camera_rotation_angle_local.y += 0.392699082;
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Y"))
		{
			camera_rotation_angle_local.y -= 0.392699082;
		}

		if (ImGui::Button("Rotate Left Z"))
		{
			camera_rotation_angle_local.z += 0.392699082;
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Z"))
		{
			camera_rotation_angle_local.z -= 0.392699082;
		}

		if (ImGui::Button("Close Me"))
		{
			camera_local_rotation_window = false;
		}

		//scene.GetActiveCamera().setSelfAngle(camera_rotation_angle_local);
		ImGui::End();
	}

	if (camera_local_translation_window)
	{
		ImGui::Begin("Camera Local Translation", &camera_local_translation_window);

		ImGui::SliderFloat("Camera X", &cameraX, -200.0, 200.0);
		ImGui::SliderFloat("Camera Y", &cameraY, -200.0, 200.0);
		ImGui::SliderFloat("Camera Z", &cameraZ, -200.0, 200.0);

		//scene.GetActiveCamera().SetCameraEye(glm::vec3(cameraX, cameraY, cameraZ));
		ImGui::End();
	}
}

void CameraWindowsWorld(const std::shared_ptr<Scene>& scene)
{
	if (camera_global_rotation_window)
	{
		ImGui::Begin("Camera World Rotation", &camera_global_rotation_window);

		if (ImGui::Button("Rotate Left X"))
		{
			camera_rotation_world.x += 0.392699082;
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate Right X"))
		{
			camera_rotation_world.x -= 0.392699082;
		}
		if (ImGui::Button("Rotate Left Y"))
		{
			camera_rotation_world.y += 0.392699082;
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Y"))
		{
			camera_rotation_world.y -= 0.392699082;
		}
		if (ImGui::Button("Rotate Left Z"))
		{
			camera_rotation_world.z += 0.392699082;
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Z"))
		{
			camera_rotation_world.z -= 0.392699082;
		}
		if (ImGui::Button("Close Me")) {
			show_global_rotation_window = false;
		}

		//scene.GetActiveCamera().setWorldRotatingAngle(camera_rotation_world);
		ImGui::End();
	}
}

void LightWindow(Scene& scene)
{
	if (light_properties_window)
	{
		ImGui::Begin("Light Properties", &light_properties_window);

		ImGui::RadioButton("Flat", &light_type_selection, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Phong", &light_type_selection, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Gouraud", &light_type_selection, 2);

		// Controls
		ImGui::ColorEdit3("Light Color", (float*)&light_color);

		if (pointOrParallelLight == 0)
		{
			ImGui::SliderFloat("Light X Pos", &light_transformation.x, -50.0, 50.0);
			ImGui::SliderFloat("Light Y Pos", &light_transformation.y, -25.0, 25.0);
			ImGui::SliderFloat("Light Z Pos", &light_transformation.z, -100.0, 100.0);
		}

		else
		{
			ImGui::SliderFloat("Light X Direction", &light_direction.x, -1, 1);
			ImGui::SliderFloat("Light Y Direction", &light_direction.y, -1, 1);
			ImGui::SliderFloat("Light Z Direction", &light_direction.z, -1, 1);
		}

		if (scene.GetLightCount() > 0)
		{
			/*scene.GetActiveLight().SetLightModel(light_type_selection);
			scene.GetActiveLight().SetColorOfMesh(light_color);
			scene.GetActiveLight().SetNewPosition(light_transformation);
			scene.GetActiveLight().SetNewLightDirection(light_direction);*/
		}

		if (ImGui::Button("Close Me")) {
			light_translation_window = false;
		}
		ImGui::End();
	}
}