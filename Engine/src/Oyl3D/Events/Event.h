#pragma once

#include "Oyl3D/oylpch.h"

#define _OYL_EVENT_SIZE 32
#define _OYL_NUM_EVENT_ARGS ((_OYL_EVENT_SIZE - 8) / 4)

#define OYL_EVENT_STRUCT(class_name, event_type, event_category, x)                      \
union class_name {                                                                       \
    class_name() : type(event_type), category(event_category), args{ 0 } {}              \
    class_name(::oyl::Event& e) { *this = *reinterpret_cast<class_name*>(&e); }          \
    operator ::oyl::Event() { return *reinterpret_cast<::oyl::Event*>(this); }           \
    struct {                                                                             \
        ::oyl::u32 args[_OYL_NUM_EVENT_ARGS];                                            \
        ::oyl::u32 type;                                                                 \
        ::oyl::u32 category;                                                             \
    };                                                                                   \
                                                                                         \
    struct x;                                                                            \
private:                                                                                 \
    struct _##class_name x;                                                              \
    static_assert(sizeof(_##class_name) <= _OYL_EVENT_SIZE);                             \
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

    union WindowResizedEvent
    {
        WindowResizedEvent() : type(TypeWindowResized), category(CategoryWindow), args{ 0 } {}
        WindowResizedEvent(::oyl::Event& e) { *this = *reinterpret_cast<WindowResizedEvent*>(&e); }
        operator ::oyl::Event() { return *reinterpret_cast<::oyl::Event*>(this); }

        struct
        {
            ::oyl::u32 args[((32 - 8) / 4)];
            ::oyl::u32 type;
            ::oyl::u32 category;
        };

        struct
        {
            i32 width;
            i32 height;
        };

    private:
        struct _WindowResizedEvent
        {
            i32 width;
            i32 height;
        };

        static_assert(sizeof(_WindowResizedEvent) <= 32);
    };

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

    // Cursor Events //////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(CursorStateRequestEvent, TypeCursorStateRequest, CategoryCursorStateRequest,
                     {
                         OylEnum state;
                     });

    
    //-Cursor Events-//////////////////////////////////////////////////////

    // Viewport Events ////////////////////////////////////////////////////
    
    OYL_EVENT_STRUCT(ViewportResizedEvent, TypeViewportResized, CategoryViewport,
                     {
                         i32 id;
                         
                         f32 width;
                         f32 height;
                     });

    OYL_EVENT_STRUCT(ViewportHandleChangedEvent, TypeViewportHandleChanged, CategoryViewport,
                     {
                         i32 handle;
                     });

    //-Viewport Events-////////////////////////////////////////////////////

    // Editor Events //////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(EditorViewportHandleChangedEvent, TypeEditorViewportHandleChanged, CategoryEditor,
                     {
                         i32 handle;
                     });

    OYL_EVENT_STRUCT(EditorViewportResizedEvent, TypeEditorViewportResized, CategoryEditor,
                     {
                         f32 width;
                         f32 height;
                     });
    
    OYL_EVENT_STRUCT(EditorEntitySelectedEvent, TypeEditorEntitySelected, CategoryEditor, 
                     {
                         entt::entity entity;
                     });

    //-Editor Events-//////////////////////////////////////////////////////

    // Physics Events /////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(PhysicsResetWorldEvent, TypePhysicsResetWorld, CategoryPhysics,
                     {
                         i32 _noArgs;
                     });

    //-Physics Events-/////////////////////////////////////////////////////
}
