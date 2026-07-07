#include "EventDispatcher.h"

#include <ranges>

namespace Oyl
{
	void
	EventDispatcher::Register(
		EventId a_eventId,
		const OnEventDelegate& a_delegate,
		int32 a_priority
	)
	{
		auto& listenerList = GetOrAddListenerMapForEventId(a_eventId);

		auto delegateIter = std::ranges::find_if(
			listenerList,
			[&](const auto& a_listenerDescriptor) -> bool
			{
				return a_listenerDescriptor.delegate == a_delegate;
			}
		);

		// If the delegate is already in the list, and has a different priority than the current requested value,
		// remove it from the list to be re-added
		if (delegateIter != listenerList.end())
		{
			if (delegateIter->priority == a_priority)
				return;

			listenerList.erase(delegateIter);
		}

		// Find the first listener with a priority less than incoming listener, and insert before that
		auto listenerIter = std::ranges::find_if(
			listenerList,
			[&](const auto& a_listenerDescriptor) -> bool
			{
				return a_listenerDescriptor.priority < a_priority;
			}
		);

		listenerList.insert(
			listenerIter,
			ListenerDescriptor {
				.delegate = a_delegate,
				.priority = a_priority,
			}
		);
	}

	void
	EventDispatcher::UnRegister(EventId a_eventId, const OnEventDelegate& a_delegate)
	{
		auto* listenerMap = TryGetListenerMapForEventId(a_eventId);
		if (!listenerMap)
		{
			return;
		}

		auto listenerIter = std::ranges::find_if(
			*listenerMap,
			[&a_delegate](const ListenerDescriptor& a_listenerDescriptor) -> bool
			{
				return a_listenerDescriptor.delegate == a_delegate;
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

			delegate.Invoke(a_event);
			if (a_event.m_cancelled)
				break;
		}
	}

	EventDispatcher::ListenerList&
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

	EventDispatcher::ListenerList*
	EventDispatcher::TryGetListenerMapForEventId(EventId a_eventId)
	{
		auto iter = m_eventListenerMap.find(a_eventId);
		if (iter == m_eventListenerMap.end())
		{
			return nullptr;
		}
		return &iter->second;
	}

	const EventDispatcher::ListenerList*
	EventDispatcher::TryGetListenerMapForEventId(EventId a_eventId) const
	{
		return const_cast<EventDispatcher*>(this)->TryGetListenerMapForEventId(a_eventId);
	}
}
