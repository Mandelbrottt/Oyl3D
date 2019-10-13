#include "oylpch.h"
#include "EventListener.h"

namespace oyl
{
    EventListener::EventListener()
        : m_postEventCallback(nullptr)
    {
    }

    EventListener::~EventListener()
    {
        m_postEventCallback = nullptr;
    }

    bool EventListener::onEvent(Ref<Event> event)
    {
        return false;
    }

    void EventListener::postEvent(UniqueRef<Event> event)
    {
        m_postEventCallback(std::move(event));
    }
}
