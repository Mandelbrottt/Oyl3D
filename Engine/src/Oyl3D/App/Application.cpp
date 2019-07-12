#include "oylpch.h"

#include "Application.h"

#include "Events/EventDispatcher.h"

#include <glfw/glfw3.h>

namespace oyl {

Application* Application::s_instance = nullptr;

Application::Application()
	: m_running(true) {

	ASSERT(!s_instance, "Application already exists!");
	s_instance = this;

	Log::init();

	m_window = std::unique_ptr<Window>(Window::create());
	m_window->setEventCallback(BIND_CALLBACK(Application::onEvent));
}

Application::~Application() {

}

void Application::pushLayer(Layer* layer) {
	m_layerStack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overlay) {
	m_layerStack.pushOverlay(overlay);
}

void Application::onEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<KeyPressEvent>([&](KeyPressEvent& event)->bool
									   {
										   if (event.getKeyCode() == GLFW_KEY_ESCAPE)
											   onWindowClose(e);
										   return true;
									   });
	dispatcher.dispatch<WindowCloseEvent>(BIND_CALLBACK(Application::onWindowClose));

	for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
		(*--it)->onEvent(e);
		if (e.handled)
			break;
	}
}

void Application::run() {
	while (m_running) {

		for (Layer* layer : m_layerStack) {
			layer->onUpdate();
		}

		m_window->onUpdate();
	}
}

bool Application::onWindowClose(Event& e) {
	m_running = false;
	return true;
}

}