#pragma once

namespace oyl
{
    class EventDispatcher;
    struct Event;
    
    class IEventListener
    {
    protected:
        static const u32 MAX_EVENT_TYPES = 256;
        static const u32 MAX_CATEGORIES  = 256;
    public:
        using Priority     = u32;
        using EventMask    = std::bitset<MAX_EVENT_TYPES>;
        using CategoryMask = std::bitset<MAX_CATEGORIES>;

    public:
        explicit IEventListener()  = default;
        virtual  ~IEventListener() = default;

        virtual bool onEvent(Ref<Event> event) = 0;

        virtual void postEvent(UniqueRef<Event> event) = 0;

        virtual const EventMask&    getEventMask() const    = 0;
        virtual const CategoryMask& getCategoryMask() const = 0;

        OYL_DEPRECATED("Use listenForEventType() instead.")
        virtual void addToEventMask(OylEnum eventType) = 0;
        OYL_DEPRECATED("Use listenForEventCategory() instead.")
        virtual void addToCategoryMask(OylEnum eventCategory) = 0;

        virtual void listenForEventType(OylEnum type)         = 0;
        virtual void listenForEventCategory(OylEnum category) = 0;
        virtual void listenForAllEvents()                     = 0;
        
        virtual void ignoreEventType(OylEnum type)         = 0;
        virtual void ignoreEventCategory(OylEnum category) = 0;
        virtual void ignoreAllEvents()                     = 0;
        
        virtual void setDispatcher(Ref<EventDispatcher> dispatcher) = 0;
    };

    class EventListener : public IEventListener
    {
        friend EventDispatcher;
    public:
        explicit EventListener();
        virtual  ~EventListener();

        bool onEvent(Ref<Event> event) override;

        void postEvent(UniqueRef<Event> event) override final;

        const EventMask&    getEventMask() const override final { return m_typeMask; }
        const CategoryMask& getCategoryMask() const override final { return m_categoryMask; }

        OYL_DEPRECATED("Use listenForEventType() instead.")
        void addToEventMask(OylEnum eventType) override final { listenForEventType(eventType); }

        OYL_DEPRECATED("Use listenForEventCategory() instead.")
        void addToCategoryMask(OylEnum eventCategory) override final { listenForEventCategory(eventCategory); }

        void listenForEventType(OylEnum type) override final;
        void listenForEventCategory(OylEnum category) override final;

        void listenForAllEvents() override final;

        void ignoreEventType(OylEnum type) override final;
        void ignoreEventCategory(OylEnum category) override final;

        void ignoreAllEvents() override final;

        void setDispatcher(Ref<EventDispatcher> dispatcher) override final;

    protected:
        EventMask    m_typeMask;
        CategoryMask m_categoryMask;

        Ref<EventDispatcher> m_dispatcher;
    };
}
