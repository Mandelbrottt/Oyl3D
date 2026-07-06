#pragma once

#include "Event.h"

#include "Core/Delegate.h"

namespace Oyl
{
	class OYL_CORE_API EventDispatcher
	{
	public:
		using ListenerId = Reflection::TypeId;
		using EventId = Reflection::TypeId;

		void
		Register(
			EventId a_eventId,
			ListenerId a_listenerId,
			const EventDelegate& a_delegate,
			int32 a_priority
		);

		void
		UnRegister(EventId a_eventId, ListenerId a_listenerId);

		void
		Dispatch(EventId a_eventId, const Event& a_event) const;

	private:
		struct ListenerDescriptor
		{
			ListenerId listenerTypeId;
			EventDelegate delegate;
			int32 priority;
		};

		using ListenerMap = std::list<ListenerDescriptor>;

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
