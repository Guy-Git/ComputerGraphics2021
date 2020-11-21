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
static float scale_factor_local = 1.0;
static glm::vec3 rotation_angle_local = glm::vec3(0);
static glm::vec3 transformation_local = glm::vec3(0);

bool show_global_rotation_window = false;
bool show_global_scale_window = false;
bool show_global_translation_window = false;
static float scale_factor_global = 1.0;
static glm::vec3 rotation_angle_global = glm::vec3(0);
static glm::vec3 transformation_global = glm::vec3(0);

bool show_model_1_window = false;
bool show_model_2_window = false;
bool show_model_3_window = false;
bool show_model_4_window = false;

bool vertex_normal = false;
bool face_normal = false;
bool show_bounding_box = false;

bool show_warning_window = false;
bool show_model_selection_window = false;

static int model_selection = 0;
static int camera_selection = 0;
static int view_selection = -1;
static int last_model_selection = 0;

glm::vec4 clear_color = glm::vec4(0.26f, 0.26f, 0.26f, 1.00f);

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
void SwitchToDifferentModelView(int modelID);
void ResetParametersValue(Scene& scene);
void SetParametersValueChangingModels(Scene& scene);
void ShowScaleRotateTranslationWindowsLocal(Scene& scene);
void ShowScaleRotateTranslationWindowsGlobal(Scene& scene);
void SetNormalsAndBoundingBox(const Scene& scene);
bool IsPositionInBoundingBox(float xPos, float yPos, Scene& scene);

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

	if (show_local_scale_window)
	{
		scale_factor_local += (-1)*yoffset;
	}
	if (show_global_scale_window)
	{
		scale_factor_global += (-1)*yoffset;
	}
}

int main(int argc, char** argv)
{
	int windowWidth = 1500, windowHeight = 900;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();
	Camera cam = Camera();
	scene.AddCamera(cam);

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
		// TODO: Set new aspect ratio
	}

	if (ImGui::IsMouseClicked(0))
	{
		if (scene.GetModelCount() > 0)
		{
			/*if (IsPositionInBoundingBox(io.MousePos.x, 1000 - io.MousePos.y, scene))
			{
				show_bounding_box = true;
			}
			else
			{
				show_bounding_box = false;
			}
			scene.GetActiveModel().SetBoundingBoxShown(show_bounding_box);*/
		}
	}

	if (!io.WantCaptureKeyboard)
	{
		// TODO: Handle keyboard events here
		if (io.KeysDown[65])
		{
			// A key is down
			// Use the ASCII table for more key codes (https://www.asciitable.com/)
		}
	}

	if (!io.WantCaptureMouse)
	{
		// TODO: Handle mouse events here
		if (io.MouseDown[0])
		{
			// Left mouse button is down
		}
	}

	renderer.ClearColorBuffer(clear_color);
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

		if (ImGui::BeginMenu("Local Trans."))
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

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("World Trans."))
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
		ImGui::EndMainMenuBar();
	}

	// camera selection window - position and window flag
	ImGui::SetNextWindowPos(ImVec2(0, 710));
	ImGui::SetNextWindowSize(ImVec2(330, 200));

	ImGui::Begin("Camera selection", &show_model_selection_window, ImGuiWindowFlags_NoMove);

	ImGui::RadioButton("Camera #1", &camera_selection, 0);
	ImGui::RadioButton("Camera #2", &camera_selection, 1);

	if (ImGui::Button("Reset Current Camera"))
	{
		view_selection = -1;
		scene.GetActiveCamera().ResetOrthographicTrans();
		scene.GetActiveCamera().ResetPerspectiveTrans();
	}

	ImGui::RadioButton("Orthographic View", &view_selection, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Perspective View", &view_selection, 1);

	ImGui::End();

	scene.SetActiveCameraIndex(camera_selection);

	switch(view_selection)
	{
	case 0:
		scene.GetActiveCamera().SetOrthographicTrans(0.5, -0.5, 0.5, -0.5, -0.1, -0.5);
		break;
	case 1:
		scene.GetActiveCamera().SetPerspectiveTrans(-0.5, 0.5, -0.5, 0.5, 0.5, -0.5);
		break;
	}

	// model selection window - position and window flag
	ImGui::SetNextWindowPos(ImVec2(330, 710));
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
	ImGui::RadioButton("Model #3", &model_selection, 2);
	if (scene.GetModelCount() > 2)
	{
		ImGui::SameLine(); ImGui::Text("[MODEL LOADED - %s]", scene.GetModel(2).GetModelName().c_str());
	}
	ImGui::RadioButton("Model #4", &model_selection, 3);
	if (scene.GetModelCount() > 3)
	{
		ImGui::SameLine(); ImGui::Text("[MODEL LOADED - %s]", scene.GetModel(3).GetModelName().c_str());
	}
	if (ImGui::Button("Reset Current Model"))
	{
		ResetParametersValue(scene);
	}
	ImGui::Checkbox("Show Vertex Normals", &vertex_normal);
	ImGui::SameLine(); ImGui::Checkbox("Show Face Normals", &face_normal);
	ImGui::Checkbox("Show Bounding Box", &show_bounding_box);

	ImGui::End();

	ShowScaleRotateTranslationWindowsLocal(scene); // all local windows declarations

	ShowScaleRotateTranslationWindowsGlobal(scene); // all global windows declarations

	SwitchToDifferentModelView(model_selection);

	SetNormalsAndBoundingBox(scene);

	if (show_model_1_window)
	{
		if (scene.GetModelCount() > 0)
		{
			show_warning_window = false;
			scene.SetActiveModelIndex(model_selection);
		}
		else {
			show_warning_window = true;
		}
	}

	if (show_model_2_window)
	{
		if (scene.GetModelCount() > 1)
		{
			show_warning_window = false;
			scene.SetActiveModelIndex(model_selection);
		}
		else {
			show_warning_window = true;
		}
	}

	if (show_model_3_window)
	{
		if (scene.GetModelCount() > 2)
		{
			show_warning_window = false;
			scene.SetActiveModelIndex(model_selection);
		}
		else {
			show_warning_window = true;
		}
	}

	if (show_model_4_window)
	{
		if (scene.GetModelCount() > 3)
		{
			show_warning_window = false;
			scene.SetActiveModelIndex(model_selection);
		}
		else {
			show_warning_window = true;
		}
	}

	SetParametersValueChangingModels(scene);


	if (show_warning_window)
	{
		ImGui::SetNextWindowPos(ImVec2(150, 300));
		ImGui::SetNextWindowSize(ImVec2(250, 100));

		ImGui::Begin("Warning!", &show_warning_window);
		ImGui::Text("Model not loaded to this selection \nPlease open model!");

		ImGui::End();
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
		ImGui::Text("Move model with arrows");

		ImGui::SliderFloat("Move X", &transformation_local.x, -50.0f, 50.0f);
		if (transformation_local.x != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}

		ImGui::SliderFloat("Move Y", &transformation_local.y, -50.0f, 50.0f);
		if (transformation_local.y != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}

		ImGui::SliderFloat("Move Z", &transformation_local.z, -50.0f, 50.0f);
		if (transformation_local.z != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}

		if (ImGui::Button("Close Me")) {
			show_local_translation_window = false;
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
		ImGui::Text("Move model with arrows");

		ImGui::SliderFloat("Move X", &transformation_global.x, -50.0f, 50.0f);
		if (transformation_global.x != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_global);
		}

		ImGui::SliderFloat("Move Y", &transformation_global.y, -50.0f, 50.0f);
		if (transformation_global.y != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_global);
		}

		ImGui::SliderFloat("Move Z", &transformation_global.z, -50.0f, 50.0f);
		if (transformation_global.z != 0)
		{
			scene.GetActiveModel().SetNewPosition(transformation_global);
		}

		if (ImGui::Button("Close Me")) {
			show_global_translation_window = false;
		}
		ImGui::End();
	}
}