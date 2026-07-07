#pragma once

#include "Event.h"

#include "Core/Delegate.h"

namespace Oyl
{
	class OYL_CORE_API EventDispatcher
	{
	public:
		using ListenerId = Reflection::TypeId;

		void
		Register(
			EventId a_eventId,
			ListenerId a_listenerId,
			const OnEventDelegate& a_delegate,
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
			OnEventDelegate delegate;
			int32 priority;
		};

		using ListenerList = std::list<ListenerDescriptor>;

	private:
		ListenerList&
		GetOrAddListenerMapForEventId(EventId a_eventId);

		ListenerList*
		TryGetListenerMapForEventId(EventId a_eventId);

		const ListenerList*
		TryGetListenerMapForEventId(EventId a_eventId) const;

	private:
		std::unordered_map<EventId, ListenerList> m_eventListenerMap;
	};
}
