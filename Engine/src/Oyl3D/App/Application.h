#pragma once

#include "Oyl3D/Common.h"
#include "Oyl3D/Events/Event.h"
#include "Oyl3D/App/Window.h"

namespace oyl {

	class Application {
	public:
		Application();
		virtual ~Application();

		void run();

		void onEvent(Event& e);
	private:
		bool onWindowClose(Event& e);

		std::unique_ptr<Window> m_window;

		bool m_running;
	};

	Application* createApplication();

}
