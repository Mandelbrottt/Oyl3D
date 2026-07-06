#pragma once

#include "Event.h"

#include "Core/Delegate.h"

namespace Oyl
{
	class OYL_CORE_API EventDispatcher
	{
	public:
		using EventId = Reflection::TypeId;
		using EventDelegate = Delegate<void(const Event&)>;

		void
		Register(EventId a_eventId, Reflection::TypeId a_listenerId, const EventDelegate& a_delegate);

		void
		UnRegister(EventId a_eventId, Reflection::TypeId a_listenerId);

		void
		Dispatch(EventId a_eventId, const Event& a_event) const;

	private:
		using ListenerMap = std::unordered_map<Reflection::TypeId, EventDelegate>;

	private:
		ListenerMap&
		GetOrAddListenerMapForEventId(EventId a_eventId);

		ListenerMap*
		TryGetListenerMapForEventId(EventId a_eventId);

		const ListenerMap*
		TryGetListenerMapForEventId(EventId a_eventId) const;

	private:
		std::unordered_map<EventId, ListenerMap> m_eventListenerMap;
	};
}
