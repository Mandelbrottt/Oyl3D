#include "oylpch.h"

#include "Application.h"

#include "Events/EventDispatcher.h"

#include "Input/Input.h"

#include <glad/glad.h>

namespace oyl {

Application* Application::s_instance = nullptr;

Application::Application()
	: m_running(true) {

	ASSERT(!s_instance, "Application already exists!");
	s_instance = this;

	Log::init();

	m_window = std::unique_ptr<Window>(Window::create());
	m_window->setEventCallback(BIND_CALLBACK(Application::onEvent));

	m_imguiLayer = new ImGuiLayer();
	pushOverlay(m_imguiLayer);
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
	dispatcher.dispatch<WindowCloseEvent>(BIND_CALLBACK(Application::onWindowClose));

	for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
		(*--it)->onEvent(e);
		if (e.handled)
			break;
	}
}

void Application::run() {
	while (m_running) {

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// TEMPORARY:
		if (Input::isKeyPressed(OYL_KEY_ESCAPE))
			m_running = false;

		for (Layer* layer : m_layerStack) 
			layer->onUpdate();

#if !defined(OYL_DIST)
		m_imguiLayer->begin();
		for (Layer* layer : m_layerStack)
			layer->onImGuiRender();
		m_imguiLayer->end();
#endif

		m_window->onUpdate();
	}
}

bool Application::onWindowClose(Event& e) {
	m_running = false;
	return true;
}

}