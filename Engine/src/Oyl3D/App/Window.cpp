#include "oylpch.h"
#include "Window.h"

#include "Rendering/Renderer.h"

#include "Platform/Windows/Win32Window.h"

namespace oyl
{
    Ref<Window> Window::create(const WindowProps& props)
    {
        switch (Renderer::getAPI())
        {
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<Window>(new Win32Window(props));
        }
        return nullptr;
    }
}
