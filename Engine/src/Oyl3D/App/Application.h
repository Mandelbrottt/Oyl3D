#pragma once

#include "Oyl3D/Common.h"
#include "Oyl3D/App/Window.h"
#include "Oyl3D/Events/Event.h"
#include "Oyl3D/System/LayerStack.h"

namespace oyl {

	class Application {
	public:
		Application();
		virtual ~Application();

		void run();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
	private:
		bool onWindowClose(Event& e);
	private:
		std::unique_ptr<Window> m_window;
		bool m_running;

		LayerStack m_layerStack;
	};

	Application* createApplication();

}
