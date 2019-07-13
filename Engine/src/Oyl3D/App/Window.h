#pragma once

#include "Oyl3D/Common.h"
#include "Oyl3D/Events/Event.h"

namespace oyl {

enum FullscreenType : char {
	Windowed,
	Borderless,
	Fullscreen
};

struct WindowProps {
	std::string title;
	unsigned int width;
	unsigned int height;
	bool vsync;
	FullscreenType type;

	WindowProps(std::string title = "Oyl3D",
				unsigned int width = 1280,
				unsigned int height = 720,
				bool vsync = true, 
				FullscreenType type = Windowed)
		: title(title), width(width), height(height), 
		  vsync(vsync), type(type) {}
};

class Window {
public:
	using EventCallbackFn = std::function<void(Event&)>;
public:
	virtual ~Window() {}

	virtual void onUpdate() = 0;

	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;

	virtual void setEventCallback(const EventCallbackFn& callback) = 0;
	
	virtual void setVsync(bool enabled) = 0;
	virtual bool isVsync() const = 0;

	virtual void setFullscreenType(FullscreenType type) = 0;
	virtual bool getFullscreenType() const = 0;

	virtual void* getNativeWindow() const = 0;

	static Window* create(const WindowProps& props = WindowProps());
};

}