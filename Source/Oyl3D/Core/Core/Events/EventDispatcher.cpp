#include "EventDispatcher.h"

#include <ranges>

namespace Oyl
{
	void
	EventDispatcher::Register(EventId a_eventId, Reflection::TypeId a_listenerId, const EventDelegate& a_delegate)
	{
		auto& listenerMap = GetOrAddListenerMapForEventId(a_eventId);

		auto listenerIter = listenerMap.find(a_listenerId);
		if (listenerIter != listenerMap.end())
		{
			listenerIter->second = a_delegate;
		} else
		{
			listenerMap.emplace(a_listenerId, a_delegate);
		}
	}

	void
	EventDispatcher::UnRegister(EventId a_eventId, Reflection::TypeId a_listenerId)
	{
		auto* listenerMap = TryGetListenerMapForEventId(a_eventId);
		if (!listenerMap)
		{
			return;
		}

		auto listenerIter = listenerMap->find(a_listenerId);
		if (listenerIter == listenerMap->end())
		{
			return;
		}

		listenerMap->erase(listenerIter);
	}

	void
	EventDispatcher::Dispatch(EventId a_eventId, const Event* a_event) const
	{
		auto listenerMap = TryGetListenerMapForEventId(a_eventId);
		if (!listenerMap)
		{
			return;
		}

		for (const auto& delegate : *listenerMap | std::views::values)
		{
			delegate.Invoke(a_event);
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
