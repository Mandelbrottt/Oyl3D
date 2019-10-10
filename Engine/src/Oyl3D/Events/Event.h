#pragma once

#include "Oyl3D/oylpch.h"

#define _OYL_EVENT_SIZE 32
#define _OYL_NUM_EVENT_ARGS ((_OYL_EVENT_SIZE - 8) / 4)

#define OYL_EVENT_STRUCT(class_name, event_type, event_category, x)               \
union class_name {                                                                \
    class_name() : type(event_type), category(event_category), args{ 0 } {}                  \
    class_name(::oyl::Event& e) { *this = *reinterpret_cast<class_name*>(&e); }   \
    operator ::oyl::Event() { return *reinterpret_cast<::oyl::Event*>(this); }    \
    struct {                                                                      \
        ::oyl::u32 args[_OYL_NUM_EVENT_ARGS];                                     \
        ::oyl::u32 type;                                                          \
        ::oyl::u32 category;                                                      \
    };                                                                            \
    struct x;                                                                     \
private:                                                                          \
    struct _sizeTest x;                                                           \
    static_assert(sizeof(_sizeTest) <= _OYL_NUM_EVENT_ARGS * 4);                  \
}

namespace oyl
{
    struct Event
    {
        u32 args[_OYL_NUM_EVENT_ARGS];
        u32 type;
        u32 category;

        static UniqueRef<Event> create(Event e)
        {
            auto sptr = UniqueRef<Event>::create();
            *sptr     = e;
            return sptr;
        }
    };

    // Window Events //////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(WindowClosedEvent, TypeWindowClosed, CategoryWindow,
                     {
                     i32 _noArgs;
                     });

    OYL_EVENT_STRUCT(WindowMovedEvent, TypeWindowMoved, CategoryWindow,
                     {
                     i32 x;
                     i32 y;
                     });

    OYL_EVENT_STRUCT(WindowResizedEvent, TypeWindowResized, CategoryWindow,
                     {
                     i32 width;
                     i32 height;
                     });

    OYL_EVENT_STRUCT(WindowFocusedEvent, TypeWindowFocused, CategoryWindow,
                     {
                     i32 focused;
                     });

    //-Window Events-//////////////////////////////////////////////////////

    // Keyboard Events ////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(KeyPressedEvent, TypeKeyPressed, CategoryKeyboard,
                     {
                     i32 keycode;
                     i32 repeatCount;
                     i32 mods;
                     });

    OYL_EVENT_STRUCT(KeyReleasedEvent, TypeKeyReleased, CategoryKeyboard,
                     {
                     i32 keycode;
                     i32 mods;

                     });

    OYL_EVENT_STRUCT(KeyTypedEvent, TypeKeyTyped, CategoryKeyboard,
                     {
                     i32 keycode;
                     i32 scancode;
                     i32 repeatCount;
                     i32 mods;
                     });

    //-Keyboard Events-////////////////////////////////////////////////////

    // Mouse Events ///////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(MousePressedEvent, TypeMousePressed, CategoryMouse,
                     {
                     i32 button;
                     i32 mods;
                     });

    OYL_EVENT_STRUCT(MouseReleasedEvent, TypeMouseReleased, CategoryMouse,
                     {
                     i32 button;
                     i32 mods;
                     });

    OYL_EVENT_STRUCT(MouseMovedEvent, TypeMouseMoved, CategoryMouse,
                     {
                     f32 x;
                     f32 y;
                     f32 dx;
                     f32 dy;
                     });

    OYL_EVENT_STRUCT(MouseScrolledEvent, TypeMouseScrolled, CategoryMouse,
                     {
                     f32 x;
                     f32 y;
                     });

    //-Mouse Events-///////////////////////////////////////////////////////

    // Gamepad Events /////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(GamepadConnectedEvent, TypeGamepadConnected, CategoryGamepad,
                     {
                     u32 gid;
                     });

    OYL_EVENT_STRUCT(GamepadDisconnectedEvent, TypeGamepadDisconnected, CategoryGamepad,
                     {
                     u32 gid;
                     });

    OYL_EVENT_STRUCT(GamepadButtonPressedEvent, TypeGamepadButtonPressed, CategoryGamepad,
                     {
                     u32 gid;

                     u32 button;
                     u32 repeatCount;
                     });

    OYL_EVENT_STRUCT(GamepadButtonReleasedEvent, TypeGamepadButtonReleased, CategoryGamepad,
                     {
                     u32 gid;

                     u32 button;
                     });

    OYL_EVENT_STRUCT(GamepadStickMovedEvent, TypeGamepadStickMoved, CategoryGamepad,
                     {
                     u32 gid;

                     u32 stick;
                     f32 x;
                     f32 y;
                     f32 dx;
                     f32 dy;
                     });

    OYL_EVENT_STRUCT(GamepadTriggerPressedEvent, TypeGamepadTriggerPressed, CategoryGamepad,
                     {
                     u32 gid;

                     u32 trigger;
                     f32 x;
                     f32 dx;
                     });

    OYL_EVENT_STRUCT(GamepadVibrationEvent, TypeGamepadVibration, CategoryGamepadVibration,
                     {
                     u32 gid;

                     f32 leftTime;
                     f32 rightTime;
                     f32 leftMotor;
                     f32 rightMotor;
                     });

    //-Gamepad Events-/////////////////////////////////////////////////////

    /*
// To create your own custom event types and type categories, create your own
// enum with any name of type unsigned int, and have the first value be equal to
// $EventTypeCustomStart or $EventCategoryCustomStart.
// ex. enum CustomEventType : unsigned int { CustomEvent1 = oyl::EventTypeCustomStart, CustomEvent2, CustomEvent3 };

// Supporting Templates for the CATEGORY_FLAGS macro
    template<int v1, int...vx> struct CategoryFlags
    {
        constexpr static int RESULT = (1 << v1) | CategoryFlags<vx...>::RESULT;
    };

    template<int v> struct CategoryFlags<v>
    {
        constexpr static int RESULT = 1 << v;
    };

#define CATEGORY_FLAGS(...) CategoryFlags<__VA_ARGS__>::RESULT

#if defined(OYL_LOG_CONSOLE)
    #define EVENT_CLASS_TYPE(type)	static int getStaticType() { return type; }\
    								virtual int getEventType() const override { return getStaticType(); }\
    								virtual const char* getName() const override { return #type; }
#else
    #define EVENT_CLASS_TYPE(type)	static int getStaticType() { return type; }\
    								virtual int getEventType() const override { return getStaticType(); }
#endif

#define EVENT_CLASS_CATEGORY(...) virtual int getCategoryFlags() const override { return CATEGORY_FLAGS(__VA_ARGS__); }

    class Event
    {
    public:
        virtual int getEventType() const = 0;
        virtual int getCategoryFlags() const = 0;

        inline bool isInCategory(int category) { return category & getCategoryFlags(); };

    #if defined(OYL_LOG_CONSOLE)
        virtual const char* getName() const = 0;
        virtual std::string toString() const = 0;
    #endif

    public:
        bool handled = false;
    };

    static std::ostream& operator <<(std::ostream& os, Event& e)
    {
        os << e;
        return os;
    }

/////////////////////////////////////////////////

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            return "WindowCloseEvent";
        }
    #endif

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

/////////////////////////////////////////////////

    class WindowMoveEvent : public Event
    {
    public:
        WindowMoveEvent(uint x, uint y)
            : m_x(x), m_y(y)
        {
        }

        inline uint getWidth() const { return m_x; }
        inline uint getHeight() const { return m_y; }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "WindowMoveEvent: (%d, %d)", m_x, m_y);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(WindowMove)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    private:
        uint m_x, m_y;
    };

/////////////////////////////////////////////////

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint width, uint height)
            : m_width(width), m_height(height)
        {
        }

        inline uint getWidth() const { return m_width; }
        inline uint getHeight() const { return m_height; }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "WindowResizeEvent: (%d, %d)", m_width, m_height);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(WindowMove)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    private:
        uint m_width;
        uint m_height;
    };

/////////////////////////////////////////////////

    class WindowFocusEvent : public Event
    {
    public:
        WindowFocusEvent(int focused)
            : m_focused(focused)
        {
        }

        inline int isFocused() { return m_focused; }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            return "WindowFocusEvent: " + m_focused ? "Gained" : "Lost";
        }
    #endif

        EVENT_CLASS_TYPE(WindowFocus)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    private:
        int m_focused;
    };

/////////////////////////////////////////////////

    class KeyEvent : public Event
    {
    public:
        inline int getKeyCode() const { return m_keyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard, EventCategoryInput)
    protected:
        KeyEvent(int keyCode)
            : m_keyCode(keyCode)
        {
        }

        int m_keyCode;
    };

/////////////////////////////////////////////////

    class KeyPressEvent : public KeyEvent
    {
    public:
        KeyPressEvent(int keyCode, int repeatCount = 0)
            : KeyEvent(keyCode), m_repeatCount(repeatCount)
        {
        }

        inline int getRepeatCount() { return m_repeatCount; }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "KeyPressEvent: %d (%d times)", m_keyCode, m_repeatCount);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(KeyPress)
    private:
        int m_repeatCount;
    };

/////////////////////////////////////////////////

    class KeyTypeEvent : public KeyEvent
    {
    public:
        KeyTypeEvent(int keyCode)
            : KeyEvent(keyCode)
        {
        }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "KeyTypeEvent: %c", (char) m_keyCode);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(KeyType)
    };

/////////////////////////////////////////////////

    class KeyReleaseEvent : public KeyEvent
    {
    public:
        KeyReleaseEvent(int keyCode, int repeatCount = 0)
            : KeyEvent(keyCode)
        {
        }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "KeyReleaseEvent: %d", m_keyCode);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(KeyRelease)
    };

/////////////////////////////////////////////////

    class MouseButtonEvent : public Event
    {
    public:
        inline int getButton() { return m_button; }

        EVENT_CLASS_CATEGORY(EventCategoryMouse, EventCategoryInput)
    protected:
        MouseButtonEvent(int button)
            : m_button(button)
        {
        }

        int m_button;
    };

/////////////////////////////////////////////////

    class MousePressEvent : public MouseButtonEvent
    {
    public:
        MousePressEvent(int button)
            : MouseButtonEvent(button)
        {
        }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "MousePressEvent: %d", m_button);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(KeyPress)
    };

/////////////////////////////////////////////////

    class MouseReleaseEvent : public MouseButtonEvent
    {
    public:
        MouseReleaseEvent(int button)
            : MouseButtonEvent(button)
        {
        }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "MouseReleaseEvent: %d", m_button);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(KeyRelease)
    };

/////////////////////////////////////////////////

    class MouseMoveEvent : public Event
    {
    public:
        MouseMoveEvent(float x, float y, float dx = 0, float dy = 0)
            : m_x(x), m_y(y), m_dx(dx), m_dy(dy)
        {
        }

        inline float getX() { return m_x; }
        inline float getY() { return m_y; }
        inline float getDX() { return m_dx; }
        inline float getDY() { return m_dy; }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "MouseMoveEvent: %f, %f", m_x, m_y);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(MouseMove)
        EVENT_CLASS_CATEGORY(EventCategoryMouse, EventCategoryInput)
    private:
        float m_x;
        float m_y;
        float m_dx;
        float m_dy;
    };

/////////////////////////////////////////////////

    class MouseScrollEvent : public Event
    {
    public:
        MouseScrollEvent(float x, float y)
            : m_x(x), m_y(y)
        {
        }

        inline float getX() { return m_x; }
        inline float getY() { return m_y; }

    #if defined(OYL_LOG_CONSOLE)
        std::string toString() const override
        {
            char buffer[100];
            sprintf(buffer, "MouseScrollEvent: %f, %f", m_x, m_y);
            return std::string(buffer);
        }
    #endif

        EVENT_CLASS_TYPE(MouseScroll)
        EVENT_CLASS_CATEGORY(EventCategoryMouse, EventCategoryInput)
    private:
        float m_x;
        float m_y;
    };
    */
}
