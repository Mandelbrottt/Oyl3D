#pragma once

#include "Oyl3D/Events/Event.h"

namespace oyl
{
    class IEventListener
    {
    protected:
        static const u32 MAX_EVENT_TYPES = 128;
        static const u32 MAX_CATEGORIES  = 32;
    public:
        using Priority = u32;
        using EventMask = std::bitset<MAX_EVENT_TYPES>;
        using CategoryMask = std::bitset<MAX_CATEGORIES>;

        using PostEventFn = std::function<void(UniqueRef<Event>)>;
        using RegisterFn = std::function<void(Ref<IEventListener>, Priority)>;
        using UnregisterFn = std::function<void(Ref<IEventListener>)>;

    public:
        explicit IEventListener()  = default;
        virtual  ~IEventListener() = default;

        virtual bool onEvent(Ref<Event> event) = 0;

        virtual void postEvent(UniqueRef<Event> event) = 0;

        virtual const EventMask&    getEventMask() const = 0;
        virtual const CategoryMask& getCategoryMask() const = 0;

        virtual void addToEventMask(u32 eventType) = 0;
        virtual void addToCategoryMask(u32 eventCategory) = 0;

        virtual void setPostEventCallback(PostEventFn callback) = 0;
        virtual void setRegisterCallback(RegisterFn callback) = 0;
        virtual void setUnregisterCallback(UnregisterFn callback) = 0;
    };

    class EventListener : public IEventListener
    {
        friend class EventDispatcher;
    public:
        explicit EventListener();
        virtual  ~EventListener();

        virtual bool onEvent(Ref<Event> event) override;

        virtual void postEvent(UniqueRef<Event> event) override;

        virtual const EventMask&    getEventMask() const override { return m_eventMask; }
        virtual const CategoryMask& getCategoryMask() const override { return m_categoryMask; }

        virtual void addToEventMask(u32 eventType) override { m_eventMask[eventType] = true; }
        virtual void addToCategoryMask(u32 eventCategory) override { m_categoryMask[eventCategory] = true; }

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
        EventMask    m_eventMask;
        CategoryMask m_categoryMask;

        PostEventFn  m_postEventCallback;
        RegisterFn   m_registerCallback;
        UnregisterFn m_unregisterCallback;
    };
}
