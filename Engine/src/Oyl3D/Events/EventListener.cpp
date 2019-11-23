#include "oylpch.h"

#include "EventDispatcher.h"
#include "EventListener.h"
#include "Event.h"

namespace oyl
{
    EventListener::EventListener()
        : m_dispatcher(nullptr) { }

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

    void EventListener::listenForEventType(OylEnum type)
    {
        m_typeMask[(u32) type] = true;
    }

    void EventListener::listenForEventCategory(OylEnum category)
    {
        m_categoryMask[(u32) category] = true;
    }

    void EventListener::listenForAllEvents()
    {
        m_typeMask.set();
        m_categoryMask.set();
    }

    void EventListener::ignoreEventType(OylEnum type)
    {
        m_typeMask[(u32) type] = false;
    }
    
    void EventListener::ignoreEventCategory(OylEnum category)
    {
        m_categoryMask[(u32) category] = false;
    }

    void EventListener::ignoreAllEvents()
    {
        m_typeMask.reset();
        m_categoryMask.reset();
    }

    void EventListener::setDispatcher(Ref<EventDispatcher> dispatcher)
    {
        m_dispatcher = std::move(dispatcher);
    }
}
