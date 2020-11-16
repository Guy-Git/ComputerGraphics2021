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
static float rotation_angle_local = 0;
static glm::vec2 transformation_local = glm::vec2(0);

bool show_global_rotation_window = false;
bool show_global_scale_window = false;
bool show_global_translation_window = false;
static float scale_factor_global = 1.0;
static float rotation_angle_global = 0;
static glm::vec2 transformation_global = glm::vec2(0);

bool show_model_1_window = false;
bool show_model_2_window = false;
bool show_model_3_window = false;
bool show_model_4_window = false;

bool show_warning_window = false;
bool show_model_selection_window = false;

static int model_selection = 0;
static int last_model_selection = 0;

glm::vec4 clear_color = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);

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

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

int main(int argc, char** argv)
{
	int windowWidth = 1000, windowHeight = 1000;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
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
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		// TODO: Set new aspect ratio
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

	// model selection window - position and window flag
	ImGui::SetNextWindowPos(ImVec2(0, 850));
	ImGui::SetNextWindowSize(ImVec2(310, 150));

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

	ImGui::End();

	ShowScaleRotateTranslationWindowsLocal(scene); // all local windows declarations

	ShowScaleRotateTranslationWindowsGlobal(scene); // all global windows declarations

	SwitchToDifferentModelView(model_selection);

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

void ResetParametersValue(Scene& scene)
{
	if (scene.GetModelCount() > 0)
	{
		scene.GetActiveModel().SetScaleFactor(1);
		scene.GetActiveModel().SetRotateAngle(0);
		scene.GetActiveModel().SetNewPosition(glm::vec2(0, 0));

		scale_factor_local = 1;
		rotation_angle_local = 0;
		transformation_local = glm::vec2(0);

		scale_factor_global = 1;
		rotation_angle_global = 0;
		transformation_global = glm::vec2(0);


		scene.SetScaleFactor(1);
		scene.SetRotateAngle(0);
		scene.SetNewPosition(glm::vec2(0, 0));
	}
}

void ShowScaleRotateTranslationWindowsLocal(Scene& scene) {
	if (show_local_rotation_window)
	{
		ImGui::Begin("Local Rotation Window", &show_local_rotation_window);
		ImGui::Text("Rotation left or right");

		if (ImGui::Button("Rotate Left"))
		{
			rotation_angle_local += 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
		}

		ImGui::SameLine();
		if (ImGui::Button("Rotate Right")) {
			rotation_angle_local -= 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
		}

		ImGui::SameLine();
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

		if (ImGui::Button("/\\"))
		{
			transformation_local.y += 1;
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}
		if (ImGui::Button("<")) {
			transformation_local.x -= 1;
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}
		ImGui::SameLine();
		if (ImGui::Button(">")) {
			transformation_local.x += 1;
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}
		if (ImGui::Button("\\/")) {
			transformation_local.y -= 1;
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

		if (ImGui::Button("Rotate Left"))
		{
			rotation_angle_global += 0.392699082;
			scene.SetRotateAngle(rotation_angle_global);
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate Right"))
		{
			rotation_angle_global -= 0.392699082;
			scene.SetRotateAngle(rotation_angle_global);
		}
		ImGui::SameLine();
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

		if (ImGui::Button("/\\"))
		{
			transformation_global.y += 5;
			scene.SetNewPosition(transformation_global);
		}
		if (ImGui::Button("<")) {
			transformation_global.x -= 5;
			scene.SetNewPosition(transformation_global);
		}
		ImGui::SameLine();
		if (ImGui::Button(">")) {
			transformation_global.x += 5;
			scene.SetNewPosition(transformation_global);
		}
		if (ImGui::Button("\\/")) {
			transformation_global.y -= 5;
			scene.SetNewPosition(transformation_global);
		}

		if (ImGui::Button("Close Me")) {
			show_global_translation_window = false;
		}
		ImGui::End();
	}
}