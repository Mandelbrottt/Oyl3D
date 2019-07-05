#pragma once

#include "Oyl3d/Events/Event.h"

namespace oyl {

class EventDispatcher {
	using EventFn = std::function<bool(Event&)>;
public:
	EventDispatcher(Event& event)
		: m_event(event) {}

	template<int EventType> bool dispatch(EventFn func) {
		if (m_event.type == EventType) {
			m_event.handled = func(m_event);
			return true;
		}
		return false;
	}
private:
	Event& m_event;
};

}