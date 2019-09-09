#pragma once

#include "Oyl3D/Common.h"
#include "Oyl3D/Events/Event.h"

namespace oyl
{
    struct WindowProps
    {
        std::string title;
        int         width;
        int         height;
        bool        vsync;
        uint        type;

        WindowProps(std::string title  = "Oyl3D",
                    int         width  = 1280,
                    int         height = 720,
                    bool        vsync  = true,
                    OylEnum     type   = Windowed)
            : title(title), width(width), height(height),
              vsync(vsync), type(type)
        {
        }
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;
    public:
        virtual ~Window() = default;

        virtual void onUpdate(bool doSwapBuffers = true) = 0;

        virtual uint getWidth() const = 0;
        virtual uint getHeight() const = 0;

        virtual void setEventCallback(const EventCallbackFn& callback) = 0;

        virtual void setVsync(bool enabled) = 0;
        virtual bool isVsync() const = 0;

        virtual void    setFullscreenType(OylEnum type) = 0;
        virtual OylEnum getFullscreenType() const = 0;

        // Isn't handled until the next frame
        virtual void updateViewport(int width, int height) = 0;

        virtual void* getNativeWindow() const = 0;

        static Ref<Window> create(const WindowProps& props = WindowProps());
    };
}
