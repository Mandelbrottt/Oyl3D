#pragma once

#include "Oyl3D/Events/Event.h"

namespace oyl
{
    class IEventListener
    {
    public:
        using Priority = u32;
        using EventMask = std::bitset<128>;
        
        using PostEventFn = std::function<void(UniqueRef<Event>)>;
        using RegisterFn = std::function<void(Ref<IEventListener>, Priority)>;
        using UnregisterFn = std::function<void(Ref<IEventListener>)>;

    public:
        explicit IEventListener() = default;
        virtual ~IEventListener() = default;

        virtual bool onEvent(Ref<Event> event) = 0;

        virtual void postEvent(UniqueRef<Event> event) = 0;

        virtual const EventMask& getEventMask() const = 0;
        
        virtual void setPostEventCallback(PostEventFn callback) = 0;
        virtual void setRegisterCallback(RegisterFn callback) = 0;
        virtual void setUnregisterCallback(UnregisterFn callback) = 0;
    };

    class EventListener : public IEventListener
    {
        friend class EventDispatcher;
    public:
        explicit EventListener();
        virtual ~EventListener();
        
        virtual bool onEvent(Ref<Event> event) override;
        
        virtual void postEvent(UniqueRef<Event> event) override;

        virtual const EventMask& getEventMask() const override { return m_eventMask; }
        
        virtual void setPostEventCallback(PostEventFn callback) override
        {
            m_postEventCallback = std::move(callback);
        }

        virtual void setRegisterCallback(RegisterFn callback) override
        {
            m_registerCallback = std::move(callback);
        }

        virtual void setUnregisterCallback(UnregisterFn callback) override
        {
            m_unregisterCallback = std::move(callback);
        }
        
    protected:
        EventMask m_eventMask;

        PostEventFn m_postEventCallback;
        RegisterFn m_registerCallback;
        UnregisterFn m_unregisterCallback;
    };
}


// OLD
namespace oyl
{
    class EventListenerDeprecated
    {
        template<class T> using EventFn = std::function<bool(T&)>;
    public:
        explicit EventListenerDeprecated(Event& event)
            : m_event(event)
        {
        }

        template<class T> bool dispatch(EventFn<T> func)
        {
            if (m_event.getEventType() == T::getStaticType())
            {
                m_event.handled = func(*(T*) &m_event);
                return true;
            }
            return false;
        }

    private:
        Event& m_event;
    };
}
