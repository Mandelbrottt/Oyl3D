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
	MousePress, MouseRelease, MouseMove, MouseScroll,
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

struct Event abstract {

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
	unsigned int x;
	unsigned int y;
	bool vsync;
	bool fullscreen;
	bool shouldClose;

#if !defined(OYL_DIST)
	inline virtual std::string toString() const final;
#endif
};

struct KeyEvent : public Event {
	int keycode;
	int count;
	char mods;
	bool pressed;

#if !defined(OYL_DIST)
	inline virtual std::string toString() const final;
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
	inline virtual std::string toString() const final;
#endif
};

struct GamePadEvent : public Event {
	char button;
	char playerID;
	bool pressed;
	float value;
	float prevValue;

#if !defined(OYL_DIST)
	inline virtual std::string toString() const final;
#endif
};

struct SoundEvent : public Event {
	//TODO: Implement sound events when audio is implemented

#if !defined(OYL_DIST)
	inline virtual std::string toString() const final;
#endif
};

#if !defined(OYL_DIST)
inline std::string WindowEvent::toString() const {
	std::stringstream ss;
	switch (type) {
	case WindowClose:
		ss << "WindowClose"; break;
	case WindowMove:
		ss << "WindowMove: " << x << "," << y; break;
	case WindowResize:
		ss << "WindowResize: " << x << "," << y; break;
	case WindowFocus:
		ss << "WindowFocus"; break;
	case WindowLostFocus:
		ss << "WindowLostFocus"; break;
	}
	return ss.str();
}

inline std::string KeyEvent::toString() const {
	std::stringstream ss;
	switch (type) {
	case KeyPress:
		ss << "KeyPress: " << keycode << "(" << count << " times)"; break;
	case KeyType:
		ss << "KeyType: " << (char) keycode; break;
	case KeyRelease:
		ss << "KeyRelease: " << keycode; break;
	}
	return ss.str();
}

inline std::string MouseEvent::toString() const {
	std::stringstream ss;
	switch (type) {
	case MousePress:
		ss << "MousePress: " << (int) button; break;
	case MouseRelease:
		ss << "MouseRelease: " << (int) button; break;
	case MouseMove:
		ss << "MouseMove: ";
		ss << "pos(" << position.x << ", " << position.y << "), ";
		ss << "delta(" << delta.x << ", " << delta.y << ")"; break;
	case MouseScroll:
		ss << "MouseScroll: (" << scroll.x << ", " << scroll.y << ")"; break;
	}
	return ss.str();
}

inline std::string GamePadEvent::toString() const {
	std::stringstream ss;
	return ss.str();
}

inline std::string SoundEvent::toString() const {
	std::stringstream ss;
	return ss.str();
}

inline std::ostream& operator<<(std::ostream& os, const Event& e) {
	os << e.toString();
	return os;
}
#endif

}