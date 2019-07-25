#pragma once

#include "Oyl3D/App/Window.h"

#include "Oyl3D/Platform/OpenGL/OpenGLContext.h"

namespace oyl {

class Win32Window : public Window {
public:
	Win32Window(const WindowProps& props);
	virtual ~Win32Window();

	void onUpdate() override;

	virtual uint getWidth() const override { return m_data.width; }
	virtual uint getHeight() const override { return m_data.height; }

	virtual void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }

	virtual void setVsync(bool enabled) override;
	virtual bool isVsync() const override;

	virtual void setFullscreenType(FullscreenType enabled) override;
	virtual FullscreenType getFullscreenType() const override;

	virtual void* getNativeWindow() const override { return m_window; }
private:
	virtual void init(const WindowProps& props);
	virtual void shutdown();
private:
	GLFWwindow* m_window;
	GraphicsContext* m_context;

	struct WindowData {
		std::string title = "Oyl3D";
		int posx = 0;
		int posy = 0;
		int width = 1280;
		int height = 720;

		GLFWmonitor* monitor = nullptr;

		bool vsync = true;
		FullscreenType fullscreenType = FullscreenType::Windowed;

		EventCallbackFn eventCallback;
	};

	WindowData m_data;

	static bool s_GLFWInitialized;
};

}
