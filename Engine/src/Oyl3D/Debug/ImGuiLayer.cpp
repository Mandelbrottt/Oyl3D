#include "oylpch.h" 
#include "ImGuiLayer.h"

#include "Oyl3D/Platform/OpenGL/ImGuiOpenGLRenderer.h"

#include "Oyl3D/App/Application.h"

#include "glfw/glfw3.h"

namespace oyl {

ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer"),
	m_time(0.0f) {

}

ImGuiLayer::~ImGuiLayer() {

}

void ImGuiLayer::onAttach() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	io.KeyMap[ImGuiKey_Tab] = OYL_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = OYL_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = OYL_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = OYL_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = OYL_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = OYL_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = OYL_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = OYL_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = OYL_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = OYL_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = OYL_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = OYL_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = OYL_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = OYL_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = OYL_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = OYL_KEY_A;
	io.KeyMap[ImGuiKey_C] = OYL_KEY_C;
	io.KeyMap[ImGuiKey_V] = OYL_KEY_V;
	io.KeyMap[ImGuiKey_X] = OYL_KEY_X;
	io.KeyMap[ImGuiKey_Y] = OYL_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = OYL_KEY_Z;

	ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::onDetach() {

}

void ImGuiLayer::onUpdate() {
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::get();
	io.DisplaySize = ImVec2((float) app.getWindow().getWidth(), (float) app.getWindow().getHeight());

	float time = (float) glfwGetTime();
	io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
	m_time = time;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	static bool show = true;
	ImGui::ShowDemoWindow(&show);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::onEvent(Event & e) {
	EventDispatcher dispatcher(e);

	dispatcher.dispatch<MouseMoveEvent>(BIND_CALLBACK(ImGuiLayer::onMouseMove));
	dispatcher.dispatch<MousePressEvent>(BIND_CALLBACK(ImGuiLayer::onMousePress));
	dispatcher.dispatch<MouseReleaseEvent>(BIND_CALLBACK(ImGuiLayer::onMouseRelease));
	dispatcher.dispatch<MouseScrollEvent>(BIND_CALLBACK(ImGuiLayer::onMouseScroll));
	dispatcher.dispatch<KeyPressEvent>(BIND_CALLBACK(ImGuiLayer::onKeyPress));
	dispatcher.dispatch<KeyReleaseEvent>(BIND_CALLBACK(ImGuiLayer::onKeyRelease));
	dispatcher.dispatch<KeyTypeEvent>(BIND_CALLBACK(ImGuiLayer::onKeyType));
}

bool ImGuiLayer::onMouseMove(MouseMoveEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(e.getX(), e.getY());

	return false;
}

bool ImGuiLayer::onMousePress(MousePressEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.getButton()] = true;

	return false;
}

bool ImGuiLayer::onMouseRelease(MouseReleaseEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.getButton()] = false;

	return false;
}

bool ImGuiLayer::onMouseScroll(MouseScrollEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH = e.getX();
	io.MouseWheel = e.getY();

	return false;
}

bool ImGuiLayer::onKeyPress(KeyPressEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[e.getKeyCode()] = true;

	return false;
}

bool ImGuiLayer::onKeyRelease(KeyReleaseEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[e.getKeyCode()] = false;

	return false;
}

bool ImGuiLayer::onKeyType(KeyTypeEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(e.getKeyCode());

	return false;
}

}

