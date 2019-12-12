#pragma once

#include "Oyl3D/App/Window.h"

#include "Oyl3D/Platform/OpenGL/OpenGLContext.h"

namespace oyl
{
    class Win32Window : public Window
    {
    public:
        explicit Win32Window(const WindowProps& props);
        virtual ~Win32Window();

        void onUpdate(bool doSwapBuffers = true) override;

        virtual uint getWidth() const override { return m_data.width; }
        virtual uint getHeight() const override { return m_data.height; }

        virtual void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }

        virtual void setVsync(bool enabled) override;
        virtual bool isVsync() const override;

        virtual void setWindowState(WindowState state) override;
        virtual WindowState getWindowState() const override;

        virtual void setCursorState(CursorState state) override;
        virtual CursorState getCursorState() const override;

        virtual void updateViewport(int width, int height) override;

        virtual void* getNativeWindow() const override { return m_window; }
    private:
        virtual void init(const WindowProps& props);
        virtual void shutdown();

        void pollGamepadEvents();
    private:
        GLFWwindow*          m_window;
        Ref<GraphicsContext> m_context;

        bool m_isViewportDirty = false;
        int  m_desiredWidth, m_desiredHeight;

        struct WindowData
        {
            std::string title  = "Oyl3D";
            int         posx   = 0;
            int         posy   = 0;
            int         width  = 1280;
            int         height = 720;
            bool        vsync  = true;

            GLFWmonitor* monitor = nullptr;

            WindowState windowState = WindowState::Windowed;

            EventCallbackFn eventCallback;
        };

        WindowData m_data;

        static bool s_GLFWInitialized;

        static struct VibrationData
        {
            f32 leftTime, rightTime;
            f32 leftVibe, rightVibe;
        } s_gamepadVibration[4];
    };
}
