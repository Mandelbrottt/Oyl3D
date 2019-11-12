#include "oylpch.h"

#include "Event.h"
#include "EventDispatcher.h"
#include "EventListener.h"

#define MAX_LISTENERS 100
#define MAX_EVENTS 1000

namespace oyl
{
    EventDispatcher::EventDispatcher()
    {
        m_eventQueue.reserve(MAX_EVENTS);

        m_listenerAddSet.reserve(MAX_LISTENERS);
        m_listenerDeleteSet.reserve(MAX_LISTENERS);
        m_listeners.reserve(MAX_LISTENERS);
    }
    
    EventDispatcher::~EventDispatcher()
    {
        for (auto& info : m_listenerAddSet)
            if (auto ref = info.listener.lock())
                ref->setDispatcher(nullptr);

        for (auto& info : m_listeners)
            if (auto ref = info.listener.lock())
                ref->setDispatcher(nullptr);

        m_listeners.clear();
        m_listenerAddSet.clear();
        m_listenerDeleteSet.clear();
    }

    void EventDispatcher::registerListener(Ref<IEventListener> listener, Priority priority)
    {
        auto it = m_listenerDeleteSet.begin();

        for (; it != m_listenerDeleteSet.end(); ++it)
        {
            if (it->lock() == listener)
                break;
        }
        if (it != m_listenerDeleteSet.end())
            m_listenerDeleteSet.erase(it);

        ListenerInfo info = { priority, listener };
        m_listenerAddSet.emplace_back(std::move(info));
    }

    void EventDispatcher::unregisterListener(Ref<IEventListener> listener)
    {
        auto it = m_listenerAddSet.begin();

        for (; it != m_listenerAddSet.end(); ++it)
        {
            if (it->listener.lock() == listener)
                break;
        }
        if (it != m_listenerAddSet.end())
            m_listenerAddSet.erase(it);

        m_listenerDeleteSet.emplace_back(std::move(listener));
    }

    void EventDispatcher::postEvent(UniqueRef<Event> event)
    {
        if (event)
        {
            UniqueRef<Event> newEvent(nullptr);
            m_eventQueue.emplace_back(std::move(event));
        }
                
    }

    void EventDispatcher::validateListeners()
    {
        {
            auto it = m_listeners.begin();
            for (; it != m_listeners.end(); ++it)
            {
                if (it->listener.expired())
                    it = m_listeners.erase(it);
            }
        }

        for (const auto& listener : m_listenerDeleteSet)
        {
            auto listenerRef = listener.lock();
            if (listenerRef) continue;

            auto it = m_listeners.begin();
            for (; it != m_listeners.end(); ++it)
                if (it->listener.lock() == listenerRef)
                {
                    it = m_listeners.erase(it);
                    break;
                }
        }

        m_listenerDeleteSet.clear();

        for (const auto& listener : m_listenerAddSet)
        {
            auto listenerRef = listener.listener.lock();
            if (!listenerRef) continue;

            if (m_listeners.empty())
            {
                m_listeners.emplace_back(listener);
                continue;
            }
            
            auto it = m_listeners.begin();            
            for (; it != m_listeners.end(); ++it)
            {
                if (it->listener.lock() == listenerRef)
                    break;

                if (listener.priority > it->priority)
                {
                    m_listeners.insert(it, listener);
                    break;
                }
            }
            if (it == m_listeners.end())
                m_listeners.emplace_back(listener);
        }

        m_listenerAddSet.clear();
    }

    void EventDispatcher::dispatchEvents()
    {
        validateListeners();

        UniqueRef<Event> event(nullptr);

        auto it = m_eventQueue.begin();
        for (; it != m_eventQueue.end(); ++it)
        {
            event = std::move(*it);
            
            dispatchEvent(std::move(event));
        }
        
        m_eventQueue.clear();
    }

    void EventDispatcher::dispatchEvent(UniqueRef<Event> event)
    {
        Ref<Event> dispatchEvent(std::move(event));

        for (const auto& listener : m_listeners)
        {
            auto li = listener.listener.lock();
            
            if (li->getEventMask()[dispatchEvent->type] ||
                li->getCategoryMask()[dispatchEvent->category])
            {
                if (li->onEvent(dispatchEvent))
                    break;
            }
        }
    }
}
