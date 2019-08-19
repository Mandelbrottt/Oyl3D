#pragma once

#include "Oyl3D/Common.h"
#include "Oyl3D/App/Window.h"
#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"
#include "Oyl3D/System/LayerStack.h"
#include "Oyl3D/Debug/ImGuiLayer.h"

#include "Oyl3D/Graphics/Buffer.h"
#include "Oyl3D/Graphics/Camera.h"

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
		inline FrameBuffer& getMainFrameBuffer() { return *m_mainBuffer; }
		inline PerspectiveCamera& getMainCamera() { return m_camera; }

		inline static Application& get() { return *s_instance; }
	private:
		bool m_running = true;
		bool m_doUpdate = true;

		uniqueRef<Window> m_window;
		
		LayerStack m_layerStack;
		ImGuiLayer* m_imguiLayer;

		ref<FrameBuffer> m_mainBuffer;

		PerspectiveCamera m_camera;

		float m_lastFrameTime = 0;
	private:
		static Application* s_instance;
	};

	Application* createApplication();

}
