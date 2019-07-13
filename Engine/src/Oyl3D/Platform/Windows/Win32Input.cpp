#include "oylpch.h"
#include "Win32Input.h"

#include "Oyl3D/App/Application.h"
#include "glfw/glfw3.h"

namespace oyl {

Input* Input::s_instance = new Win32Input();

bool Win32Input::isKeyPressedImpl(int keyCode) {
	auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
	auto state = glfwGetKey(window, keyCode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Win32Input::isMousePressedImpl(int button) {
	auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
	auto state = glfwGetMouseButton(window, button);
	return state == GLFW_PRESS;
}

glm::vec2 Win32Input::getMousePosImpl() {
	auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return {(float) xpos, (float) ypos};
}

float Win32Input::getMouseXImpl() {
	return getMousePosImpl().x;
}

float Win32Input::getMouseYImpl() {
	return getMousePosImpl().y;
}

}

