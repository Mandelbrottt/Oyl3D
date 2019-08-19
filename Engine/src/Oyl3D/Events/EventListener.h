#pragma once

#include "Oyl3d/Events/Event.h"

namespace oyl {

class EventListener {
	template<class T> using EventFn = std::function<bool(T&)>;
public:
	EventListener(Event& event)
		: m_event(event) {}

	template<class T> bool dispatch(EventFn<T> func) {
		if (m_event.getEventType() == T::getStaticType()) {
			m_event.handled = func(*(T*)&m_event);
			return true;
		}
		return false;
	}
private:
	Event& m_event;
};

}