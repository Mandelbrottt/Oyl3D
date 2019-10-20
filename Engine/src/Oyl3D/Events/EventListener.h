#pragma once

namespace oyl
{
    class EventDispatcher;
    struct Event;
    
    class IEventListener
    {
    protected:
        static const u32 MAX_EVENT_TYPES = 128;
        static const u32 MAX_CATEGORIES  = 32;
    public:
        using Priority = u32;
        using EventMask = std::bitset<MAX_EVENT_TYPES>;
        using CategoryMask = std::bitset<MAX_CATEGORIES>;

    public:
        explicit IEventListener()  = default;
        virtual  ~IEventListener() = default;

        virtual bool onEvent(Ref<Event> event) = 0;

        virtual void postEvent(UniqueRef<Event> event) = 0;

        virtual const EventMask&    getEventMask() const = 0;
        virtual const CategoryMask& getCategoryMask() const = 0;

        virtual void addToEventMask(u32 eventType) = 0;
        virtual void addToCategoryMask(u32 eventCategory) = 0;

        virtual void setDispatcher(Ref<EventDispatcher> dispatcher) = 0;
    };

    class EventListener : public IEventListener
    {
        friend EventDispatcher;
    public:
        explicit EventListener();
        virtual  ~EventListener();

        virtual bool onEvent(Ref<Event> event) override;

        virtual void postEvent(UniqueRef<Event> event) override final;
        
        virtual const EventMask&    getEventMask() const override final { return m_eventMask; }
        virtual const CategoryMask& getCategoryMask() const override final { return m_categoryMask; }

        // TODO: Change u32 to OylEnum
        virtual void addToEventMask(u32 eventType) override final { m_eventMask[eventType] = true; }
        virtual void addToCategoryMask(u32 eventCategory) override final { m_categoryMask[eventCategory] = true; }

        virtual void setDispatcher(Ref<EventDispatcher> dispatcher) override final;

    protected:
        EventMask    m_eventMask;
        CategoryMask m_categoryMask;

        Ref<EventDispatcher> m_dispatcher;
    };
}
