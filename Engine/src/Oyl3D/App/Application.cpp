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

	void Application::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowClose>(BIND_CALLBACK(Application::onWindowClose));

		LOG("{0}", e);
	}

	bool Application::onWindowClose(Event& e) {
		m_running = false;
		return true;
	}

	void Application::run() {
		while (m_running) {
			m_window->onUpdate();
		}
	}

}