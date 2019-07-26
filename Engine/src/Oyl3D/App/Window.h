#pragma once

#include "Oyl3D/Common.h"
#include "Oyl3D/Events/Event.h"

namespace oyl {

enum class FullscreenType : char {
	Windowed,
	Borderless,
	Fullscreen = Borderless
};

struct WindowProps {
	std::string title;
	int width;
	int height;
	bool vsync;
	FullscreenType type;

	WindowProps(std::string title = "Oyl3D",
				int width = 1280,
				int height = 720,
				bool vsync = true, 
				FullscreenType type = FullscreenType::Windowed)
		: title(title), width(width), height(height), 
		  vsync(vsync), type(type) {}
};

class Window {
public:
	using EventCallbackFn = std::function<void(Event&)>;
public:
	virtual ~Window() {}

	virtual void onUpdate() = 0;

	virtual uint getWidth() const = 0;
	virtual uint getHeight() const = 0;

	virtual void setEventCallback(const EventCallbackFn& callback) = 0;
	
	virtual void setVsync(bool enabled) = 0;
	virtual bool isVsync() const = 0;

	virtual void setFullscreenType(FullscreenType type) = 0;
	virtual FullscreenType getFullscreenType() const = 0;

	virtual void updateViewport() = 0;

	virtual void* getNativeWindow() const = 0;

	static Window* create(const WindowProps& props = WindowProps());
};

}