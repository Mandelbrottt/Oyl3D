#pragma once

#include "Oyl3D/App/Window.h"

#include <GLFW/glfw3.h>

namespace oyl {

class Win32Window : public Window {
public:
	Win32Window(const WindowProps& props);
	virtual ~Win32Window();

	void onUpdate() override;

	virtual unsigned int getWidth() const override { return m_data.width; }
	virtual unsigned int getHeight() const override { return m_data.height; }

	virtual void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }

	virtual void setVsync(bool enabled) override;
	virtual bool isVsync() const override;

	virtual void setFullscreenType(FullscreenType enabled) override;
	virtual bool getFullscreenType() const override;
private:
	virtual void init(const WindowProps& props);
	virtual void shutdown();
private:
	GLFWwindow* m_window;

	struct WindowData {
		std::string title;
		unsigned int width;
		unsigned int height;
		bool vsync;
		FullscreenType fullscreenType;

		EventCallbackFn eventCallback;
	};

	WindowData m_data;

	static bool s_GLFWInitialized;
};

}
