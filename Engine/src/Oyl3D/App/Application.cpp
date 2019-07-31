#include "oylpch.h"

#include "Application.h"

#include "Events/EventDispatcher.h"

#include "Input/Input.h"

#include "System/Platform.h"
#include "System/Timestep.h"

#include "Rendering/Renderer.h"

#include <imgui.h>

namespace oyl {

Application* Application::s_instance = nullptr;

Application::Application()
	: m_camera(60.0f, 16.0f / 9.0f, 0.01f, 1000.0f) {

	ASSERT(!s_instance, "Application already exists!");
	s_instance = this;

	Log::init();

	m_window = std::unique_ptr<Window>(Window::create());
	m_window->setEventCallback(BIND_CALLBACK(Application::onEvent));

	m_imguiLayer = new ImGuiLayer();
	pushOverlay(m_imguiLayer);

	m_mainBuffer.reset(oyl::FrameBuffer::create(1));
	m_mainBuffer->initDepthTexture(m_window->getWidth(), m_window->getHeight());

	m_mainBuffer->initColorTexture(0, m_window->getWidth(), m_window->getHeight(),
								   oyl::RGBA8,
								   oyl::Nearest,
								   oyl::Clamp);

	m_window->setVsync(false);
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
	dispatcher.dispatch<WindowCloseEvent>([&](WindowCloseEvent & e)->bool 
										  {
											  m_running = false;
											  return false;
										  });
	dispatcher.dispatch<WindowResizeEvent>([&](WindowResizeEvent & e)->bool
										   {
											   m_window->updateViewport(e.getWidth(), e.getHeight());
											   m_mainBuffer->updateViewport(e.getWidth(), e.getHeight());
											   return false;
										   });
	dispatcher.dispatch<WindowFocusEvent>([&](WindowFocusEvent & e)->bool
										  {
#if defined(OYL_DIST)
											  m_doUpdate = e.isFocused();
#endif
											  return !e.isFocused();
										  });
	dispatcher.dispatch<KeyReleaseEvent>([&](KeyReleaseEvent & e)->bool {
		static bool lmao = false;
		if (e.getKeyCode() == Key_F7)
			m_window->setVsync(lmao = !lmao);
		return false;
										 });

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

		if (m_doUpdate) {
			RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			RenderCommand::clear();
			m_mainBuffer->clear();

			m_mainBuffer->bind();
			Renderer::beginScene(m_camera);

			for (Layer* layer : m_layerStack)
				layer->onUpdate(abs(timestep) > 1.0f / 30.0f ? 1.0f / 30.0f : timestep);

			Renderer::endScene();
			m_mainBuffer->unbind();
		}

		// HACK: Still using demo window, also abstract away into ImGuiLayer
		Timestep updateTime = (float) Platform::getTime() - time;

#if !defined(OYL_DIST)
			//RenderCommand::clear();

			m_imguiLayer->begin();
			for (Layer* layer : m_layerStack)
				layer->onImGuiRender();

			ImGui::Begin("Viewport");

			auto[x, y] = ImGui::GetWindowSize();

			m_camera.setAspect(x / y);

			ImGui::Image(
				(void*) m_mainBuffer->getColorHandle(0),
				ImVec2(x - 15, y - 35),
				ImVec2(0, 1), ImVec2(1, 0)
			);

			int distance = 10;

			auto [wx, wy] = ImGui::GetWindowPos();

			ImVec2 window_pos = ImVec2(wx + distance, wy + distance + 20);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0, 0));
	
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
				
			bool debugOpen = true;
			auto flags = (ImGuiWindowFlags_NoMove |
						  ImGuiWindowFlags_NoDocking |
						  ImGuiWindowFlags_NoTitleBar |
						  ImGuiWindowFlags_NoResize |
						  ImGuiWindowFlags_AlwaysAutoResize |
				 		  ImGuiWindowFlags_NoSavedSettings |
						  ImGuiWindowFlags_NoFocusOnAppearing |
						  ImGuiWindowFlags_NoNav
						  );
			ImGui::Begin("DebugInfo", &debugOpen, flags);
			ImGui::Text("Debug Info");

			static float ut = updateTime.getMillis();
			static float ts = timestep.getMillis();
			static float totalTime = timestep;
			totalTime += timestep;
			if (totalTime > 1) {
				ut = updateTime.getMillis();
				ts = timestep.getMillis();
				totalTime = 0;
			}

			ImGui::Text("  Render Time: %.2fms ", ut);
			ImGui::Text("  Frame Time:  %.2fms", ts);
			ImGui::End();
			
			ImGui::End();

			m_imguiLayer->end();

			//m_window->updateViewport(lmao.x, lmao.y);
			//m_mainBuffer->updateViewport(lmao.x, lmao.y);
#else
		m_mainBuffer->moveToBackBuffer(m_window->getWidth(), m_window->getHeight());
#endif
		
		m_window->onUpdate(m_doUpdate);
		m_lastFrameTime = time;
	}
}

}