#include "oylpch.h"

#include "EventDispatcher.h"
#include "EventListener.h"
#include "Event.h"

namespace oyl
{
    EventListener::EventListener()
        : m_dispatcher(nullptr)
    {
    }

    EventListener::~EventListener()
    {
        m_dispatcher.reset();
    }

    bool EventListener::onEvent(Ref<Event> event)
    {
        return false;
    }

    void EventListener::postEvent(UniqueRef<Event> event)
    {
        m_dispatcher->postEvent(std::move(event));
    }

    void EventListener::setDispatcher(Ref<EventDispatcher> dispatcher)
    {
        m_dispatcher = std::move(dispatcher);
    }
}
