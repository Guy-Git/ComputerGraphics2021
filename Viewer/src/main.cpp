#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>
#include <iostream>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"

/**
 * Fields
 */
bool show_demo_window = false;
bool show_another_window = false;

bool show_local_rotation_window = false;
bool show_local_scale_window = false;
bool show_local_translation_window = false;

bool camera_local_rotation_window = false;
bool camera_local_translation_window = false;

bool light_translation_window = false;

static float scale_factor_local = 1.0;
static glm::vec3 rotation_angle_local = glm::vec3(0);
static glm::vec3 transformation_local = glm::vec3(0);

static glm::vec3 camera_rotation_angle_local = glm::vec3(0);
static glm::vec3 camera_transformation_local = glm::vec3(0);
static glm::vec3 camera_rotation_world = glm::vec3(0);

int light_type_selection = 0;

bool show_global_rotation_window = false;
bool show_global_scale_window = false;
bool show_global_translation_window = false;
bool camera_global_rotation_window = false;

static float scale_factor_global = 1.0;
static glm::vec3 rotation_angle_global = glm::vec3(0);
static glm::vec3 transformation_global = glm::vec3(0);

bool show_model_1_window = false;
bool show_model_2_window = false;
bool show_model_3_window = false;
bool show_model_4_window = false;

bool light_properties_window = false;
bool model_materials_window = false;

bool vertex_normal = false;
bool face_normal = false;
bool show_bounding_box = false;

bool rotate_light = false;

bool show_warning_window = false;
bool show_model_selection_window = false;

static int model_selection = 0;
static int light_selection = 0;
static int camera_selection = 0;
static int view_selection = 0;
static int last_model_selection = 0;

static float left = 0.1;
static float right = 10.0;
static float bottom = 0.1;
static float top = 10.0;
static float near_param = -0.1;
static float far_param = -50.0;
static float fov = 45.0;

static float cameraX = 0;
static float cameraY = 0;
static float cameraZ = 450.0;

glm::vec3 light_transformation = glm::vec3(0);

static float orthoZoom = 0.0;

float lastX = 750, lastY = 450;
bool firstMouse = true;
float yaw = 0;
float pitch = 0;

glm::vec3 model_color = glm::vec3(0.156, 0.592, 0.976);

float ambient = 0.5f;
float diffuse = 0.5f;
float specular = 0.5f;

bool texture_mapping = 0;

glm::vec3 clear_color = glm::vec3(0.26f, 0.26f, 0.26f);
glm::vec3 light_color = glm::vec3(1);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/**
 * Function declarations
 */
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene);
void ChangeCameraSelection(Scene& scene);
void ChangeLightSelection(Scene& scene);
void SwitchToDifferentModelView(int modelID);
void ResetParametersValue(Scene& scene);
void ResetLightParametersValue(Scene& scene);
void SetParametersValueChangingModels(Scene& scene);
void SetParametersValueChangingLights(Scene& scene);
void ShowScaleRotateTranslationWindowsLocal(Scene& scene);
void ShowScaleRotateTranslationWindowsGlobal(Scene& scene);
void SetNormalsAndBoundingBox(const Scene& scene);
bool IsPositionInBoundingBox(float xPos, float yPos, Scene& scene);
void CameraWindowsLocal(Scene& scene);
void CameraWindowsWorld(Scene& scene);
void LightWindow(Scene& scene);

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

	if (show_local_scale_window)
	{
		scale_factor_local += (-1) * yoffset;
	}
	if (show_global_scale_window)
	{
		scale_factor_global += (-1) * yoffset;
	}

	if (view_selection == 1)
	{
		fov -= (float)yoffset;
		if (fov < 0.1f) {
			fov = 0.1f;
		}
		if (fov > 89.0f) {
			fov = 89.0f;
		}
	}
	if (view_selection == 0)
	{
		if (right >= 0 && top >= 0)
		{
			right -= (float)yoffset;
			top -= (float)yoffset;
		}
	}
}

int main(int argc, char** argv)
{
	GLFWwindow* window = SetupGlfwWindow(1500, 900, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();

	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene);
		RenderFrame(window, scene, renderer, io);
	}

	Cleanup(window);
	return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	glm::vec3 cameraEyeBuffer = scene.GetActiveCamera().GetCameraEye();
	glm::vec3 cameraAtBuffer = scene.GetActiveCamera().GetCameraAt();
	glm::vec3 cameraUpBuffer = scene.GetActiveCamera().GetCameraUp();

	if (scene.GetModelCount() > 0)
	{
		scene.GetActiveModel().SetScaleFactor(scale_factor_local);
	}
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		scene.SetWindowSizes(frameBufferHeight, frameBufferWidth);
	}
	renderer.SetViewport(scene.GetHeight(), scene.GetWidth(), clear_color);

	const float cameraSpeed = 100.0f; // adjust accordingly
	if (cameraAtBuffer.x == 0.0 && cameraAtBuffer.y == 0.0 && cameraAtBuffer.z == 0.0)
	{
		cameraAtBuffer = glm::vec3(0.0, 0.0, 0.1);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraEyeBuffer -= glm::vec3(0.0f, 0.0f, 10.0f) * cameraSpeed / 6.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraEyeBuffer += glm::vec3(0.0f, 0.0f, 10.0f) * cameraSpeed / 6.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraEyeBuffer -= glm::vec3(0.0f, 10.0f, 0.0f) * cameraSpeed / 6.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraEyeBuffer += glm::vec3(0.0f, 10.0f, 0.0f) * cameraSpeed / 6.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraEyeBuffer += glm::normalize(glm::cross(cameraAtBuffer, cameraUpBuffer)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraEyeBuffer -= glm::normalize(glm::cross(cameraAtBuffer, cameraUpBuffer)) * cameraSpeed;
	}

	if (!io.WantCaptureMouse)
	{

		if (io.MouseDown[0])
		{
			cameraAtBuffer.x += io.MouseDelta.x; // change cameraAt with mouse
			cameraAtBuffer.y += io.MouseDelta.y;
		}
	}

	cameraX = cameraEyeBuffer.x;
	cameraY = cameraEyeBuffer.y;
	cameraZ = cameraEyeBuffer.z;

	scene.GetActiveCamera().SetCameraEye(glm::vec3(cameraX, cameraY, cameraZ));

	renderer.Render(scene);
	renderer.SwapBuffers();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

bool IsPositionInBoundingBox(float xPos, float yPos, Scene& scene)
{
	if (xPos <= scene.GetActiveModel().GetMinMax().x && xPos >= scene.GetActiveModel().GetMinMax().y &&
		yPos <= scene.GetActiveModel().GetMinMax().z && yPos >= scene.GetActiveModel().GetMinMax().w)
	{
		return true;
	}
	return false;
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (scene.GetModelCount() < 4)
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
					nfdchar_t* outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
					if (result == NFD_OKAY)
					{
						scene.AddModel(Utils::LoadMeshModel(outPath));

						model_selection = scene.GetModelCount() - 1;

						scene.GetModel(model_selection).SetColorOfMesh(glm::vec3(0.156, 0.592, 0.976));

						scene.GetModel(model_selection).kindOfModel = 0;

						//Utils::printVerticesAndFace(scene.GetModel(0));
						free(outPath);
					}
					else if (result == NFD_CANCEL)
					{
					}
					else
					{
					}
				}
				ImGui::EndMenu();
			}
		}

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

		if (ImGui::BeginMenu("Light"))
		{
			light_properties_window = true;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Post Processing"))
		{
			if (ImGui::MenuItem("Gaussing Blur + Bloom"))
			{
				scene.SetPPMode(1);
			}

			if (ImGui::MenuItem("TBD"))
			{
				scene.SetPPMode(2);
			}
			if (ImGui::MenuItem("Reset PP"))
			{
				scene.SetPPMode(0);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// camera selection window - position and window flag
	ImGui::SetNextWindowPos(ImVec2(0, scene.GetHeight() - 200));
	ImGui::SetNextWindowSize(ImVec2(330, 200));

	ImGui::Begin("Camera selection", &show_model_selection_window, ImGuiWindowFlags_NoMove);

	ImGui::RadioButton("Camera #1", &camera_selection, 0);
	ChangeCameraSelection(scene);
	ImGui::RadioButton("Camera #2", &camera_selection, 1);

	if (ImGui::Button("Reset Current Camera"))
	{
		view_selection = -1;
		scene.GetActiveCamera().ResetProjectionsMatrix();
		scene.GetActiveCamera().ResetPerspectiveTrans();
	}

	ImGui::RadioButton("Orthographic View", &view_selection, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Perspective View", &view_selection, 1);

	if (ImGui::Button("Reset Camera Position"))
	{
		scene.GetActiveCamera().ResetCameraPosition();
		cameraX = 0;
		cameraY = 0;
		cameraZ = 450.0;
		fov = 45.0;

		//zoom

	}

	/*ImGui::SliderFloat("Rotate Y:", &rotateCameraY, -180, 180);
	scene.GetActiveCamera().setSelfAngle(rotateCameraY);*/
	if (view_selection == 1)
		ImGui::SliderFloat("Fovy", &fov, 0.1, 100);
	else
		ImGui::SliderFloat("Zoom", &orthoZoom, -10, 10);

	ImGui::End();

	/*

	glm::vec3 cameraAtBuffer = scene.GetActiveCamera().GetCameraAt();

	ImGui::SliderFloat("At X", &cameraAtBuffer.x, -200.0f, 200.0f);
	ImGui::SliderFloat("At Y", &cameraAtBuffer.y, -200.0f, 200.0f);
	ImGui::SliderFloat("At Z", &cameraAtBuffer.z, -200.0f, 200.0f);

	scene.GetActiveCamera().SetCameraAt(cameraAtBuffer);

	ImGui::SliderFloat("Width", &windowWidth, 1.0, 2000.0);
	ImGui::SliderFloat("Height", &windowHeight, 1.0, 2000.0);

	if (ImGui::Button("Reset Camera Position"))
	{
		scene.GetActiveCamera().ResetCameraPosition();
	}

	ImGui::End();*/

	scene.SetActiveCameraIndex(camera_selection);

	switch (view_selection)
	{
	case 0:
		scene.GetActiveCamera().ResetProjectionsMatrix();

		scene.GetActiveCamera().SetOrthographicTrans(left, right + orthoZoom, bottom, top + orthoZoom, near_param, far_param);

		scene.GetActiveCamera().SetCameraLookAt(scene.GetActiveCamera().GetCameraEye(),
			scene.GetActiveCamera().GetCameraAt(),
			scene.GetActiveCamera().GetCameraUp());

		break;
	case 1:
		scene.GetActiveCamera().ResetProjectionsMatrix();
		scene.GetActiveCamera().SetPerspectiveTrans(glm::radians(fov), 1, near_param, far_param);
		scene.GetActiveCamera().SetCameraLookAt(scene.GetActiveCamera().GetCameraEye(),
			scene.GetActiveCamera().GetCameraAt(),
			scene.GetActiveCamera().GetCameraUp());

		break;
	}

	// model selection window - position and window flag
	ImGui::SetNextWindowPos(ImVec2(330, scene.GetHeight() - 200));
	ImGui::SetNextWindowSize(ImVec2(330, 200));

	ImGui::Begin("Model selection", &show_model_selection_window, ImGuiWindowFlags_NoMove);

	ImGui::RadioButton("Model #1", &model_selection, 0);
	if (scene.GetModelCount() > 0)
	{
		ImGui::SameLine(); ImGui::Text("[MODEL LOADED - %s]", scene.GetModel(0).GetModelName().c_str());
	}
	ImGui::RadioButton("Model #2", &model_selection, 1);
	if (scene.GetModelCount() > 1)
	{
		ImGui::SameLine(); ImGui::Text("[MODEL LOADED - %s]", scene.GetModel(1).GetModelName().c_str());
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
	ImGui::SetNextWindowPos(ImVec2(660, scene.GetHeight() - 200));
	ImGui::SetNextWindowSize(ImVec2(330, 200));

	ImGui::Begin("Light selection", &show_model_selection_window, ImGuiWindowFlags_NoMove);

	ImGui::RadioButton("Light #1", &light_selection, 0);
	if (scene.GetLightCount() > 0)
	{
		ImGui::SameLine(); ImGui::Text("[LIGHT LOADED]");
	}
	ImGui::RadioButton("Light #2", &light_selection, 1);
	if (scene.GetLightCount() > 1)
	{
		ImGui::SameLine(); ImGui::Text("[LIGHT LOADED]");
	}
	ChangeLightSelection(scene);

	if (ImGui::Button("Reset Current light"))
	{
		ResetLightParametersValue(scene);
	}

	if (ImGui::Button("Add Point Of Light"))
	{
		scene.AddLight(Utils::LoadMeshModel("../Data/cube.obj"));

		light_selection = scene.GetLightCount() - 1;
		
		scene.GetLight(light_selection).SetColorOfMesh(glm::vec3(1, 1, 1));

		scene.GetLight(light_selection).kindOfModel = 1; // point
	}

	if (ImGui::Button("Add Parallel Light"))
	{
		scene.AddLight(Utils::LoadMeshModel("../Data/cube.obj"));

		light_selection = scene.GetLightCount() - 1;

		scene.GetLight(light_selection).SetColorOfMesh(glm::vec3(1, 1, 1));

		scene.GetLight(light_selection).kindOfModel = 2; // parallel
	}

	ImGui::Checkbox("Rotate Light", &rotate_light);

	if (rotate_light)
	{
		ResetParametersValue(scene);
		scene.GetLight(light_selection).isLightRotating_ = rotate_light;
	}

	ImGui::End();

	ShowScaleRotateTranslationWindowsLocal(scene); // all local windows declarations

	ShowScaleRotateTranslationWindowsGlobal(scene); // all global windows declarations

	SwitchToDifferentModelView(model_selection);

	CameraWindowsLocal(scene);

	CameraWindowsWorld(scene);

	LightWindow(scene);

	SetNormalsAndBoundingBox(scene);

	if (show_model_1_window)
	{
		if (scene.GetModelCount() > 0)
			scene.SetActiveModelIndex(model_selection);
	}

	if (show_model_2_window)
	{
		if (scene.GetModelCount() > 1)
			scene.SetActiveModelIndex(model_selection);
	}

	SetParametersValueChangingModels(scene);
	SetParametersValueChangingLights(scene);
}

void ChangeCameraSelection(Scene& scene)
{
	if (camera_selection == 0)
	{
		scene.SetActiveCameraIndex(0);
	}
	if (camera_selection == 1)
	{
		scene.SetActiveCameraIndex(1);
	}
}

void ChangeLightSelection(Scene& scene)
{
	if (light_selection == 0)
	{
		scene.SetActiveLightIndex(0);
	}
	if (light_selection == 1)
	{
		scene.SetActiveLightIndex(1);
	}
}

void SwitchToDifferentModelView(int modelID) {
	switch (modelID)
	{
	case 0:
		show_model_1_window = true;
		show_model_2_window = false;
		show_model_3_window = false;
		show_model_4_window = false;
		break;
	case 1:
		show_model_1_window = false;
		show_model_2_window = true;
		show_model_3_window = false;
		show_model_4_window = false;
		break;
	case 2:
		show_model_1_window = false;
		show_model_2_window = false;
		show_model_3_window = true;
		show_model_4_window = false;
		break;
	case 3:
		show_model_1_window = false;
		show_model_2_window = false;
		show_model_3_window = false;
		show_model_4_window = true;
		break;
	default:
		break;
	}
}

void SetParametersValueChangingModels(Scene& scene)
{
	if (scene.GetModelCount() > 0)
	{
		scale_factor_local = scene.GetActiveModel().GetScaleFactor();
		rotation_angle_local = scene.GetActiveModel().GetRotateAngle();
		transformation_local = scene.GetActiveModel().GetPosition();

		scale_factor_global = scene.GetScaleFactor();
		rotation_angle_global = scene.GetRotateAngle();
		transformation_global = scene.GetPosition();
	}
}

void SetParametersValueChangingLights(Scene& scene)
{
	if (scene.GetLightCount() > 0)
	{
		light_transformation = scene.GetActiveLight().GetPosition();
	}
}

void SetNormalsAndBoundingBox(const Scene& scene)
{
	if (scene.GetModelCount() > 0)
	{
		scene.GetActiveModel().SetVertexNormalShown(vertex_normal);

		scene.GetActiveModel().SetFaceNormalShown(face_normal);

		scene.GetActiveModel().SetBoundingBoxShown(show_bounding_box);
	}
}

void ResetParametersValue(Scene& scene)
{
	if (scene.GetModelCount() > 0)
	{
		vertex_normal = false;
		face_normal = false;
		show_bounding_box = false;

		scene.GetActiveModel().SetScaleFactor(1);
		scene.GetActiveModel().SetRotateAngle(glm::vec3(0, 0, 0));
		scene.GetActiveModel().SetNewPosition(glm::vec3(0, 0, 0));

		scale_factor_local = 1;
		rotation_angle_local = glm::vec3(0);;
		transformation_local = glm::vec3(0);

		scale_factor_global = 1;
		rotation_angle_global = glm::vec3(0);;
		transformation_global = glm::vec3(0);


		scene.SetScaleFactor(1);
		scene.SetRotateAngle(glm::vec3(0, 0, 0));
		scene.SetNewPosition(glm::vec3(0, 0, 0));
	}
}

void ResetLightParametersValue(Scene& scene)
{
	if (scene.GetLightCount() > 0)
	{
		light_transformation = glm::vec3(0);
		scene.GetActiveLight().SetNewPosition(light_transformation);
	}
}

void ShowScaleRotateTranslationWindowsLocal(Scene& scene) {
	if (show_local_rotation_window)
	{
		ImGui::Begin("Local Rotation Window", &show_local_rotation_window);
		ImGui::Text("Choose axis and start rotate");

		if (ImGui::Button("Rotate Left X"))
		{
			rotation_angle_local.x += 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right X")) {
			rotation_angle_local.x -= 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
		}

		if (ImGui::Button("Rotate Left Y"))
		{
			rotation_angle_local.y += 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Y")) {
			rotation_angle_local.y -= 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
		}
		if (ImGui::Button("Rotate Left Z"))
		{
			rotation_angle_local.z += 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Z")) {
			rotation_angle_local.z -= 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
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

		ImGui::SliderFloat("Scale factor", &scale_factor_local, 0.0f, 2.0f);
		if (scale_factor_local != 1)
		{
			scene.GetActiveModel().SetScaleFactor(scale_factor_local);
		}

		if (ImGui::Button("Close Me")) {
			show_local_scale_window = false;
		}
		ImGui::End();
	}

	if (show_local_translation_window)
	{
		ImGui::Begin("Local Translation Window", &show_local_translation_window);

		ImGui::SliderFloat("Move X", &transformation_local.x, -10.0f, 10.0f);
		if (transformation_local.x != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}

		ImGui::SliderFloat("Move Y", &transformation_local.y, -10.0f, 10.0f);
		if (transformation_local.y != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}

		ImGui::SliderFloat("Move Z", &transformation_local.z, -10.0f, 10.0f);
		if (transformation_local.z != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_local);
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
			ambient = 0.5;
			diffuse = 0.6;
			specular = 0.9;
		}

		ImGui::SameLine();
		if (ImGui::Button("Plastic"))
		{
			ambient = 0.3;
			diffuse = 0.7;
			specular = 0.2;
		}

		ImGui::SameLine();
		if (ImGui::Button("Rubber"))
		{
			ambient = 0.8;
			diffuse = 0.4;
			specular = 0.1;
		}

		ImGui::ColorEdit3("Model Color", (float*)&model_color);

		ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse", &diffuse, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f);

		ImGui::Checkbox("Texture Mapping", &texture_mapping);

		if (scene.GetModelCount() > 0)
		{
			scene.GetActiveModel().SetAmbient(ambient);
			scene.GetActiveModel().SetDiffuse(diffuse);
			scene.GetActiveModel().SetSpecular(specular);
			scene.GetActiveModel().SetColorOfMesh(model_color);
		}

		ImGui::End();
	}
}

void ShowScaleRotateTranslationWindowsGlobal(Scene& scene) {
	if (show_global_rotation_window)
	{
		ImGui::Begin("Global Rotation Window", &show_global_rotation_window);
		ImGui::Text("Rotation left or right");

		if (ImGui::Button("Rotate Left X"))
		{
			rotation_angle_global.x += 0.392699082;
			scene.SetRotateAngle(rotation_angle_global);
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate Right X"))
		{
			rotation_angle_global.x -= 0.392699082;
			scene.SetRotateAngle(rotation_angle_global);
		}
		if (ImGui::Button("Rotate Left Y"))
		{
			rotation_angle_global.y += 0.392699082;
			scene.SetRotateAngle(rotation_angle_global);
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Y"))
		{
			rotation_angle_global.y -= 0.392699082;
			scene.SetRotateAngle(rotation_angle_global);
		}
		if (ImGui::Button("Rotate Left Z"))
		{
			rotation_angle_global.z += 0.392699082;
			scene.SetRotateAngle(rotation_angle_global);
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate Right Z"))
		{
			rotation_angle_global.z -= 0.392699082;
			scene.SetRotateAngle(rotation_angle_global);
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

		ImGui::SliderFloat("Scale factor", &scale_factor_global, 0.0f, 2.0f);
		if (scale_factor_global != 1)
		{
			scene.SetScaleFactor(scale_factor_global);
		}

		if (ImGui::Button("Close Me")) {
			show_global_scale_window = false;
		}
		ImGui::End();
	}

	if (show_global_translation_window)
	{
		ImGui::Begin("Global Translation Window", &show_global_translation_window);

		ImGui::SliderFloat("Move X", &transformation_global.x, -200.0f, 200.0f);
		if (transformation_global.x != 0)
		{
			scene.SetNewPosition(transformation_global);
		}

		ImGui::SliderFloat("Move Y", &transformation_global.y, -200.0f, 200.0f);
		if (transformation_global.y != 0)
		{
			scene.SetNewPosition(transformation_global);
		}

		ImGui::SliderFloat("Move Z", &transformation_global.z, -200.0f, 200.0f);
		if (transformation_global.z != 0)
		{
			scene.SetNewPosition(transformation_global);
		}

		if (ImGui::Button("Close Me")) {
			show_global_translation_window = false;
		}
		ImGui::End();
	}
}

void CameraWindowsLocal(Scene& scene)
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

		scene.GetActiveCamera().setSelfAngle(camera_rotation_angle_local);
		ImGui::End();
	}

	if (camera_local_translation_window)
	{
		ImGui::Begin("Camera Local Translation", &camera_local_translation_window);

		ImGui::SliderFloat("Camera X", &cameraX, -1500.0, 1500.0);
		ImGui::SliderFloat("Camera Y", &cameraY, -1500.0, 1500.0);
		ImGui::SliderFloat("Camera Z", &cameraZ, -1500.0, 1500.0);

		scene.GetActiveCamera().SetCameraEye(glm::vec3(cameraX, cameraY, cameraZ));
		ImGui::End();
	}
}

void CameraWindowsWorld(Scene& scene)
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

		scene.GetActiveCamera().setWorldRotatingAngle(camera_rotation_world);
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

		ImGui::SliderFloat("Light X Pos", &light_transformation.x, -50.0, 50.0);
		ImGui::SliderFloat("Light Y Pos", &light_transformation.y, -25.0, 25.0);
		ImGui::SliderFloat("Light Z Pos", &light_transformation.z, -100.0, 100.0);

		if (scene.GetLightCount() > 0)
		{
			scene.GetActiveLight().SetLightModel(light_type_selection);
			scene.GetActiveLight().SetColorOfMesh(light_color);
			scene.GetActiveLight().SetNewPosition(light_transformation);
		}

		if (ImGui::Button("Close Me")) {
			light_translation_window = false;
		}
		ImGui::End();
	}
}
