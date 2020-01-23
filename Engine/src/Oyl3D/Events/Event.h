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
        ::oyl::i32 args[_OYL_NUM_EVENT_ARGS];                                            \
        ::oyl::EventType type;                                                           \
        ::oyl::EventCategory category;                                                   \
    };                                                                                   \
                                                                                         \
    struct x;                                                                            \
private:                                                                                 \
    struct _##class_name x;                                                              \
    static_assert(sizeof(_##class_name) <= _OYL_EVENT_SIZE);                             \
}

namespace oyl
{
    enum class EventType : i32
    {
        WindowClosed = 0, WindowMoved, WindowResized, WindowFocused,

        ViewportResized, ViewportHandleChanged,

        KeyPressed, KeyTyped, KeyReleased,

        MousePressed, MouseReleased, MouseMoved, MouseScrolled,

        GamepadConnected, GamepadDisconnected,
        GamepadButtonPressed, GamepadButtonReleased,
        GamepadStickMoved, GamepadTriggerPressed,
        GamepadVibrationRequest,

        CursorStateRequest,
        
        PhysicsTriggerEnter, PhysicsTriggerExit, PhysicsTriggerStay,
        PhysicsCollisionEnter, PhysicsCollisionExit, PhysicsCollisionStay,
        PhysicsResetWorld,
        
        EditorViewportResized, EditorViewportHandleChanged, EditorEntitySelected,
        EditorCameraChanged, EditorCameraMoveRequest,

        CustomStart,
    };

    enum class EventCategory : i32
    {   
        Application = 0,
        Window,
        Viewport,
        Keyboard,
        Mouse,
        Gamepad,
        GamepadVibration,
        Cursor,
        Audio,
        Physics,
        Editor,
        CustomStart,
    };
    
    struct Event
    {
        i32 args[_OYL_NUM_EVENT_ARGS];
        EventType type;
        EventCategory category;
    };
    
    template<class T>
    static const T& event_cast(const Event& event)
    {
        return reinterpret_cast<const T&>(event);
    }
    
    // Window Events //////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(WindowClosedEvent, EventType::WindowClosed, EventCategory::Window,
                     {
                         i32 _noArgs;
                     });

    OYL_EVENT_STRUCT(WindowMovedEvent, EventType::WindowMoved, EventCategory::Window,
                     {
                         i32 x;
                         i32 y;
                     });

    OYL_EVENT_STRUCT(WindowResizedEvent, EventType::WindowResized, EventCategory::Window,
                     {
                         f32 width;
                         f32 height;
                     });
    
    OYL_EVENT_STRUCT(WindowFocusedEvent, EventType::WindowFocused, EventCategory::Window,
                     {
                         i32 focused;
                     });

    //-Window Events-//////////////////////////////////////////////////////

    // Keyboard Events ////////////////////////////////////////////////////

    enum class Key;
    
    OYL_EVENT_STRUCT(KeyPressedEvent, EventType::KeyPressed, EventCategory::Keyboard,
                     {
                         Key keycode;
                         i32 repeatCount;
                         i32 mods;
                     });

    OYL_EVENT_STRUCT(KeyReleasedEvent, EventType::KeyReleased, EventCategory::Keyboard,
                     {
                         Key keycode;
                         i32 mods;
                     });

    OYL_EVENT_STRUCT(KeyTypedEvent, EventType::KeyTyped, EventCategory::Keyboard,
                     {
                         Key keycode;
                         i32 scancode;
                         i32 repeatCount;
                         i32 mods;
                     });

    //-Keyboard Events-////////////////////////////////////////////////////

    // Mouse Events ///////////////////////////////////////////////////////

    enum class Mouse;

    OYL_EVENT_STRUCT(MousePressedEvent, EventType::MousePressed, EventCategory::Mouse,
                     {
                         Mouse button;
                         i32 mods;
                     });

    OYL_EVENT_STRUCT(MouseReleasedEvent, EventType::MouseReleased, EventCategory::Mouse,
                     {
                         Mouse button;
                         i32 mods;
                     });

    OYL_EVENT_STRUCT(MouseMovedEvent, EventType::MouseMoved, EventCategory::Mouse,
                     {
                         f32 x;
                         f32 y;
                         f32 dx;
                         f32 dy;
                     });

    OYL_EVENT_STRUCT(MouseScrolledEvent, EventType::MouseScrolled, EventCategory::Mouse,
                     {
                         f32 x;
                         f32 y;
                     });

    //-Mouse Events-///////////////////////////////////////////////////////

    // Gamepad Events /////////////////////////////////////////////////////

    enum class Gamepad;

    OYL_EVENT_STRUCT(GamepadConnectedEvent, EventType::GamepadConnected, EventCategory::Gamepad,
                     {
                         u32 gid;
                     });

    OYL_EVENT_STRUCT(GamepadDisconnectedEvent, EventType::GamepadDisconnected, EventCategory::Gamepad,
                     {
                         u32 gid;
                     });

    OYL_EVENT_STRUCT(GamepadButtonPressedEvent, EventType::GamepadButtonPressed, EventCategory::Gamepad,
                     {
                         u32 gid;
                         
                         Gamepad button;
                         u32 repeatCount;
                     });

    OYL_EVENT_STRUCT(GamepadButtonReleasedEvent, EventType::GamepadButtonReleased, EventCategory::Gamepad,
                     {
                        u32 gid;

                        Gamepad button;
                     });

    OYL_EVENT_STRUCT(GamepadStickMovedEvent, EventType::GamepadStickMoved, EventCategory::Gamepad,
                     {
                         u32 gid;
                         
                         Gamepad stick;
                         f32 x;
                         f32 y;
                         f32 dx;
                         f32 dy;
                     });

    OYL_EVENT_STRUCT(GamepadTriggerPressedEvent, EventType::GamepadTriggerPressed, EventCategory::Gamepad,
                     {
                         u32 gid;
                         
                         Gamepad trigger;
                         f32 x;
                         f32 dx;
                     });

    OYL_EVENT_STRUCT(GamepadVibrationRequestEvent, EventType::GamepadVibrationRequest, EventCategory::GamepadVibration,
                     {
                         u32 gid;
                         
                         f32 leftTime;
                         f32 rightTime;
                         f32 leftMotor;
                         f32 rightMotor;
                     });

    //-Gamepad Events-/////////////////////////////////////////////////////

    // Cursor Events //////////////////////////////////////////////////////

    enum class CursorState;

    OYL_EVENT_STRUCT(CursorStateRequestEvent, EventType::CursorStateRequest, EventCategory::Cursor,
                     {
                         CursorState state;
                     });

    
    //-Cursor Events-//////////////////////////////////////////////////////

    // Viewport Events ////////////////////////////////////////////////////
    
    OYL_EVENT_STRUCT(ViewportResizedEvent, EventType::ViewportResized, EventCategory::Viewport,
                     {
                         i32 id;
                         
                         f32 width;
                         f32 height;
                     });

    OYL_EVENT_STRUCT(ViewportHandleChangedEvent, EventType::ViewportHandleChanged, EventCategory::Viewport,
                     {
                         i32 handle;
                     });

    //-Viewport Events-////////////////////////////////////////////////////

    // Editor Events //////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(EditorViewportHandleChangedEvent, EventType::EditorViewportHandleChanged, EventCategory::Editor,
                     {
                         i32 handle;
                     });

    OYL_EVENT_STRUCT(EditorViewportResizedEvent, EventType::EditorViewportResized, EventCategory::Editor,
                     {
                         f32 width;
                         f32 height;
                     });
    
    OYL_EVENT_STRUCT(EditorEntitySelectedEvent, EventType::EditorEntitySelected, EventCategory::Editor, 
                     {
                         entt::entity entity;
                     });

    class Camera;
    
    OYL_EVENT_STRUCT(EditorCameraChangedEvent, EventType::EditorCameraChanged, EventCategory::Editor,
                     {
                         Ref<Camera>* camera;
                     });

    OYL_EVENT_STRUCT(EditorCameraMoveRequestEvent, EventType::EditorCameraMoveRequest, EventCategory::Editor,
                     {
                         bool doMove;
                     });

    //-Editor Events-//////////////////////////////////////////////////////

    // Physics Events /////////////////////////////////////////////////////

    OYL_EVENT_STRUCT(PhysicsTriggerEnterEvent, EventType::PhysicsTriggerEnter, EventCategory::Physics,
                     {
                         entt::entity entity1;
                         entt::entity entity2;
                     });

    OYL_EVENT_STRUCT(PhysicsTriggerExitEvent, EventType::PhysicsTriggerExit, EventCategory::Physics,
                     {
                         entt::entity entity1;
                         entt::entity entity2;
                     });

    OYL_EVENT_STRUCT(PhysicsTriggerStayEvent, EventType::PhysicsTriggerStay, EventCategory::Physics,
                     {
                         entt::entity entity1;
                         entt::entity entity2;
                     });

    OYL_EVENT_STRUCT(PhysicsCollisionEnterEvent, EventType::PhysicsCollisionEnter, EventCategory::Physics,
                     {
                         entt::entity entity1;
                         entt::entity entity2;
                     });
    
    OYL_EVENT_STRUCT(PhysicsCollisionExitEvent, EventType::PhysicsCollisionExit, EventCategory::Physics,
                     {
                         entt::entity entity1;
                         entt::entity entity2;
                     });

    OYL_EVENT_STRUCT(PhysicsCollisionStayEvent, EventType::PhysicsCollisionStay, EventCategory::Physics,
                     {
                         entt::entity entity1;
                         entt::entity entity2;
                     });

    OYL_EVENT_STRUCT(PhysicsResetWorldEvent, EventType::PhysicsResetWorld, EventCategory::Physics,
                     {
                         i32 _noArgs;
                     });

    //-Physics Events-/////////////////////////////////////////////////////
}
