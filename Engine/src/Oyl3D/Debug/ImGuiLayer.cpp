#include "oylpch.h" 
#include "ImGuiLayer.h"

#include "Oyl3D/App/Application.h"

#include <imgui.h>

#define IMGUI_IMPL_API
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <glfw/glfw3.h>
#include <glad/glad.h>

namespace oyl {

ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer") {

}

ImGuiLayer::~ImGuiLayer() {

}

void ImGuiLayer::onAttach() {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void) io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	Application& app = Application::get();
	GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 420");
}

void ImGuiLayer::onDetach() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::end() {
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::get();
	io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ImGuiLayer::onImGuiRender() {
	static bool show = true;
	ImGui::ShowDemoWindow(&show);

	bool neededBool = false;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport", &neededBool, ImGuiWindowFlags_AlwaysAutoResize |
				 ImGuiWindowFlags_NoScrollbar |
				 ImGuiWindowFlags_NoCollapse |
				 ImGuiWindowFlags_NoMove);

	auto [x, y] = ImGui::GetWindowSize();

	Application::get().getMainCamera().setAspect(x / y);

	ImGui::Image(
		(void*) Application::get().getMainFrameBuffer().getColorHandle(0),
		ImVec2(x, y),
		ImVec2(0, 1), ImVec2(1, 0)
	);

	ImGui::End();
	ImGui::PopStyleVar();
}

}

