#pragma once

#include "Oyl3D/Common.h"
#include "Oyl3D/App/Window.h"
#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventDispatcher.h"
#include "Oyl3D/System/LayerStack.h"
#include "Oyl3D/Debug/ImGuiLayer.h"

namespace oyl {

	class Application {
	public:
		Application();
		virtual ~Application();

		void run();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		inline Window& getWindow() { return *m_window; }

		inline static Application& get() { return *s_instance; }
	private:
		bool onWindowClose(Event& e);
	private:
		bool m_running = true;

		std::unique_ptr<Window> m_window;
		
		LayerStack m_layerStack;
		ImGuiLayer* m_imguiLayer;

		float m_lastFrameTime = 0;
	private:
		static Application* s_instance;
	};

	Application* createApplication();

}
