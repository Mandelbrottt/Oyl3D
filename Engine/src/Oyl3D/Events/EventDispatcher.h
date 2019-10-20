#pragma once

//#include "EventListener.h"

namespace oyl
{
    class IEventListener;
    struct Event;

    class OYLAPI EventDispatcher final
    {
    public:
        using Priority = u32;

        struct ListenerInfo
        {
            Priority                priority = 0;
            WeakRef<IEventListener> listener;
        };

    public:
        explicit EventDispatcher();
        ~EventDispatcher();

        void registerListener(Ref<IEventListener> listener, Priority priority = 0);
        void unregisterListener(Ref<IEventListener> listener);

        void postEvent(UniqueRef<Event> event);

        void dispatchEvents();

    private:
        void validateListeners();

        void dispatchEvent(UniqueRef<Event> event);

    private:
        std::vector<ListenerInfo> m_listeners;

        std::vector<ListenerInfo>            m_listenerAddSet;
        std::vector<WeakRef<IEventListener>> m_listenerDeleteSet;

        // TODO: Implement as ring list or pool allocator
        std::vector<UniqueRef<Event>> m_eventQueue;
    };
}
