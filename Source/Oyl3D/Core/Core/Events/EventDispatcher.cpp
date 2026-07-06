#include "EventDispatcher.h"

#include <ranges>

namespace Oyl
{
	void
	EventDispatcher::Register(
		EventId a_eventId,
		ListenerId a_listenerId,
		const EventDelegate& a_delegate,
		int32 a_priority
	)
	{
		auto& listenerList = GetOrAddListenerMapForEventId(a_eventId);

		// Search for ListenerList for the given EventId
		auto listenerIter = std::ranges::find_if(
			listenerList,
			[a_listenerId](const auto& a_listenerDescriptor) -> bool
			{
				return a_listenerDescriptor.listenerTypeId == a_listenerId;
			}
		);
		if (listenerIter != listenerList.end())
		{
			listenerIter->delegate = a_delegate;
			return;
		}

		// Find the first listener with a priority less than incoming listener, and insert before that
		auto priorityIter = std::ranges::find_if(
			listenerList,
			[&](const auto& a_listenerDescriptor) -> bool
			{
				return a_listenerDescriptor.priority < a_priority;
			}
		);

		listenerList.insert(
			priorityIter,
			ListenerDescriptor {
				.listenerTypeId = a_listenerId,
				.delegate = a_delegate,
				.priority = a_priority,
			}
		);
	}

	void
	EventDispatcher::UnRegister(EventId a_eventId, Reflection::TypeId a_listenerId)
	{
		auto* listenerMap = TryGetListenerMapForEventId(a_eventId);
		if (!listenerMap)
		{
			return;
		}

		auto listenerIter = std::ranges::find_if(
			*listenerMap,
			[a_listenerId](const auto& a_listenerDescriptor) -> bool
			{
				return a_listenerDescriptor.listenerTypeId == a_listenerId;
			}
		);
		if (listenerIter == listenerMap->end())
		{
			return;
		}

		listenerMap->erase(listenerIter);
	}

	void
	EventDispatcher::Dispatch(EventId a_eventId, const Event& a_event) const
	{
		auto listenerMap = TryGetListenerMapForEventId(a_eventId);
		if (!listenerMap)
		{
			return;
		}

		// Iterate all EventListeners, stop iterating if any return false
		for (const auto& descriptor : *listenerMap)
		{
			const auto& delegate = descriptor.delegate;

			bool shouldContinuePropagating = delegate.Invoke(a_event);
			if (!shouldContinuePropagating)
				break;
		}
	}

	EventDispatcher::ListenerMap&
	EventDispatcher::GetOrAddListenerMapForEventId(EventId a_eventId)
	{
		auto iter = m_eventListenerMap.find(a_eventId);
		if (iter == m_eventListenerMap.end())
		{
			auto [iterInner, emplaced] = m_eventListenerMap.try_emplace(a_eventId);
			iter = iterInner;
		}

		return iter->second;
	}

	EventDispatcher::ListenerMap*
	EventDispatcher::TryGetListenerMapForEventId(EventId a_eventId)
	{
		auto iter = m_eventListenerMap.find(a_eventId);
		if (iter == m_eventListenerMap.end())
		{
			return nullptr;
		}
		return &iter->second;
	}

	const EventDispatcher::ListenerMap*
	EventDispatcher::TryGetListenerMapForEventId(EventId a_eventId) const
	{
		return const_cast<EventDispatcher*>(this)->TryGetListenerMapForEventId(a_eventId);
	}
}
