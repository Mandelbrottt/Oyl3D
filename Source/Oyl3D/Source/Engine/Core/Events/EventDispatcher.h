#pragma once

#include <list>
#include <unordered_map>

#include "Event.h"

#include "Core/Containers/Delegate.h"

namespace Oyl
{
	class CORE_EXPORT EventDispatcher
	{
	public:
		using ListenerId = TypeId;

		void
		Register(
			EventId a_eventId,
			const OnEventDelegate& a_delegate,
			int32 a_priority
		);

		void
		Unregister(EventId a_eventId, const OnEventDelegate& a_delegate);

		void
		Dispatch(EventId a_eventId, const Event& a_event) const;

	private:
		struct ListenerDescriptor
		{
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
