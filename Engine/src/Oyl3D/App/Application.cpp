#include "oylpch.h"

#include "Application.h"

namespace oyl {

	Application::Application() 
		:m_running(true) {

		Log::init();
	}

	Application::~Application() {

	}

	void Application::run() {
		while (m_running) {

		}
	}

}