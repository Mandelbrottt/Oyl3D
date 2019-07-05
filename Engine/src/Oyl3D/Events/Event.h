#pragma once

#include "Oyl3d/oylpch.h"

namespace oyl {

// To create your own custom event types and type categories, create your own
// enum with any name of type int, and have the first value be equal to
// $EventTypeCustomStart or $EventCategoryCustomStart.
// ex. enum CustomEventType : int { CustomEvent1 = oyl::EventTypeCustomStart, CustomEvent2, CustomEvent3 }
enum EventType : int {
	None = 0,
	WindowClose, WindowMove, WindowResize, WindowFocus, WindowLostFocus,
	KeyPress, KeyType, KeyRelease,
	MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll,
	EventTypeCustomStart
};

enum EventCategory : int {
	EventCategoryApplication,
	EventCategoryWindow,
	EventCategoryInput,
	EventCategoryKeyboard,
	EventCategoryMouse,
	EventCategoryMouseButton,
	EventCategoryCustomStart
};

// Supporting Templates for the CATEGORY_FLAGS macro
template <int v1, int... vx> struct CategoryFlags {
	constexpr static int RESULT = (1 << v1) | CategoryFlags<vx...>::RESULT;
};
template <int v> struct CategoryFlags<v> {
	constexpr static int RESULT = 1 << v;
};

#define CATEGORY_FLAGS(...) CategoryFlags<__VA_ARGS__>::RESULT

struct Event {

#if !defined(OYL_DIST)
	virtual std::string toString() const = 0;
#endif

	inline bool isInCategory(int category) { return categoryFlags & category; }

	int type;
	int categoryFlags;
	bool handled;
};

struct WindowEvent : public Event {
	std::string title;
	unsigned int width;
	unsigned int height;
	bool vsync;
	bool fullscreen;
	bool shouldClose;

#if !defined(OYL_DIST)
	std::string toString() const final {
		return std::string();
	}
#endif
};

struct KeyEvent : public Event {
	int keycode;
	int count;
	char mods;
	bool pressed;

#if !defined(OYL_DIST)
	std::string toString() const final {
		return std::string();
	}
#endif
};

struct MouseEvent : public Event {
	glm::vec2 scroll;
	glm::vec2 position;
	glm::vec2 delta;
	char button;
	char mods;
	bool pressed;
	bool locked;

#if !defined(OYL_DIST)
	std::string toString() const final {
		return std::string();
	}
#endif
};

struct GamePadEvent : public Event {
	char button;
	char playerID;
	bool pressed;
	float value;
	float prevValue;

#if !defined(OYL_DIST)
	std::string toString() const final {
		return std::string();
	}
#endif
};

struct SoundEvent : public Event {
	//TODO: Implement sound events when audio is implemented

#if !defined(OYL_DIST)
	std::string toString() const final {
		return std::string();
	}
#endif
};

}