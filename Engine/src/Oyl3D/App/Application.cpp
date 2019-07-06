#include "oylpch.h"

#include "Application.h"

#include "Events/EventDispatcher.h"

namespace oyl {

	Application::Application() 
		: m_running(true) {

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
		dispatcher.dispatch<WindowClose>(BIND_CALLBACK(Application::onWindowClose));

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