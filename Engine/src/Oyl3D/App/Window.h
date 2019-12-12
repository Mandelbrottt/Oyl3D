#pragma once

#include "Oyl3D/Common.h"
#include "Oyl3D/Events/Event.h"

namespace oyl
{
    enum class WindowState : i32
    {
        Windowed,
        Fullscreen,
        Borderless = Fullscreen,
    };

    enum class CursorState : i32
    {
        Normal,
        Disabled,
        Hidden,
    };
    
    struct WindowProps
    {
        std::string title;
        int         width;
        int         height;
        bool        vsync;
        WindowState  type;

        WindowProps(std::string title  = "Oyl3D",
                    int         width  = 1280,
                    int         height = 720,
                    bool        vsync  = true,
                    WindowState  type   = WindowState::Windowed)
            : title(std::move(title)),
              width(width), height(height),
              vsync(vsync), type(type) {}
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(const Event&)>;
    public:
        virtual ~Window() = default;

        virtual void onUpdate(bool doSwapBuffers = true) = 0;

        virtual uint getWidth() const = 0;
        virtual uint getHeight() const = 0;

        virtual void setEventCallback(const EventCallbackFn& callback) = 0;

        virtual void setVsync(bool enabled) = 0;
        virtual bool isVsync() const = 0;

        virtual void setWindowState(WindowState state) = 0;
        virtual WindowState getWindowState() const = 0;

        virtual CursorState getCursorState() const = 0;
        virtual void setCursorState(CursorState state) = 0;

        // Isn't handled until the next frame
        virtual void updateViewport(int width, int height) = 0;

        virtual void* getNativeWindow() const = 0;

        static Ref<Window> create(const WindowProps& props = {});
    };
}
