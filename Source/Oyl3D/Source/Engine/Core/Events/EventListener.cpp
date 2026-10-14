#include "EventListener.h"

#include "EventDispatcher.h"

namespace Oyl
{
	EventListener::~EventListener()
	{
		m_eventDispatcher = nullptr;
	}

	void
	EventListener::RegisterEventListener(EventId a_eventId, const OnEventDelegate& a_delegate, int32 a_priority)
	{
		m_eventDispatcher->Register(a_eventId, a_delegate, a_priority);
	}

	void
	EventListener::UnregisterEventListener(EventId a_eventId, const OnEventDelegate& a_delegate)
	{
		m_eventDispatcher->Unregister(a_eventId, a_delegate);
	}

	void
	EventListener::PostEvent(const Event& a_event) const
	{
		OYL_PROFILE_FUNCTION();

		m_eventDispatcher->Dispatch(a_event.eventType, a_event);
	}

	void
	EventListener::OnRegisterEventDispatcher(EventDispatcher* a_dispatcher)
	{
		m_eventDispatcher = a_dispatcher;
	}

	void
	Internal::AttachEventListenerToEventDispatcher(EventListener* a_eventListener, EventDispatcher* a_dispatcher)
	{
		a_eventListener->OnRegisterEventDispatcher(a_dispatcher);
	}
}
