#include "oylpch.h"

#include "Application.h"

#include "Events/EventDispatcher.h"

#include "Input/Input.h"

#include "System/Platform.h"
#include "System/Timestep.h"

#include "Rendering/Renderer.h"

namespace oyl {

Application* Application::s_instance = nullptr;

Application::Application() {

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

	for (Layer* layer : m_layerStack) {
		layer->onEvent(e);
		if (e.handled)
			break;
	}
}

void Application::run() {
	while (m_running) {
		float time = (float) Platform::getTime();
		Timestep timestep(time - m_lastFrameTime);
		m_lastFrameTime = time;

		// TEMPORARY: Escape should not close the engine later on
		if (Input::isKeyPressed(OYL_KEY_ESCAPE))
			m_running = false;

		for (Layer* layer : m_layerStack)
			layer->onUpdate(abs(timestep.getSeconds()) > 1.0f / 60.0f ? Timestep(1.0f / 60.0f) : timestep);

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