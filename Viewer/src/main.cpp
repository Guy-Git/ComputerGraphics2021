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

	ImGui::End();

	//1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
   //if (show_demo_window)
   //	ImGui::ShowDemoWindow(&show_demo_window);

   ////2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
   //{
   //	static float f = 0.0f;
   //	static int counter = 0;

   //	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

   //	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
   //	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
   //	ImGui::Checkbox("Another Window", &show_another_window);

   //	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
   //	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

   //	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
   //		counter++;
   //	ImGui::SameLine();
   //	ImGui::Text("counter = %d", counter);

   //	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
   //	ImGui::End();
   //}

   // 3. Show another simple window.
   //if (show_another_window)
   //{
   //	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
   //	ImGui::Text("Hello from another window!");
   //	if (ImGui::Button("Close Me"))
   //		show_another_window = false;
   //	ImGui::End();
   //}

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

void ShowScaleRotateTranslationWindowsLocal(Scene &scene) {
	if (show_local_rotation_window)
	{
		//ImGui::SetNextWindowPos(ImVec2(150, 300));
		//ImGui::SetNextWindowSize(ImVec2(250, 200));
		ImGui::Begin("Local Rotation Window", &show_local_rotation_window);
		ImGui::Text("Rotation left or right");

		if (ImGui::Button("Rotate Left")) 
		{
			rotation_angle_local -= 0.392699082;
			scene.GetActiveModel().SetRotateAngle(rotation_angle_local);
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate Right")) {
			rotation_angle_local += 0.392699082;
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
		//ImGui::SetNextWindowPos(ImVec2(150, 300));
		//ImGui::SetNextWindowSize(ImVec2(250, 200));
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
			transformation_local.y+=20;
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}
		if (ImGui::Button("<")) {
			transformation_local.x -= 20;
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}
		ImGui::SameLine();
		if (ImGui::Button(">")) {
			transformation_local.x += 20;
			scene.GetActiveModel().SetNewPosition(transformation_local);
		}
		if (ImGui::Button("\\/")) {
			transformation_local.y -= 20;
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
		ImGui::SetNextWindowPos(ImVec2(150, 300));
		ImGui::SetNextWindowSize(ImVec2(250, 200));
		ImGui::Begin("Global Rotation Window", &show_global_rotation_window);
		ImGui::Text("Rotation left or right");

		if (ImGui::Button("rotate left")) {

		}
		ImGui::SameLine();
		if (ImGui::Button("rotate right")) {

		}
		ImGui::SameLine();
		if (ImGui::Button("Close Me")) {
			show_global_rotation_window = false;
		}
		ImGui::End();
	}

	if (show_global_scale_window)
	{
		ImGui::SetNextWindowPos(ImVec2(150, 300));
		ImGui::SetNextWindowSize(ImVec2(250, 200));
		ImGui::Begin("Global Scale Window", &show_global_scale_window);
		ImGui::Text("Scale Model");

		ImGui::SliderFloat("Scale factor", &scale_factor_global, 0.0f, 10.0f);

		if (ImGui::Button("Close Me")) {
			show_global_scale_window = false;
		}
		ImGui::End();
	}

	if (show_global_translation_window)
	{
		ImGui::SetNextWindowPos(ImVec2(150, 300));
		ImGui::SetNextWindowSize(ImVec2(250, 200));
		ImGui::Begin("Global Translation Window", &show_global_translation_window);
		ImGui::Text("Move model with arrows");

		if (ImGui::Button("/\\")) {

		}
		if (ImGui::Button("<")) {

		}
		ImGui::SameLine();
		if (ImGui::Button(">")) {

		}
		if (ImGui::Button("\\/")) {

		}
		if (ImGui::Button("Close Me")) {
			show_global_translation_window = false;
		}
		ImGui::End();
	}
}