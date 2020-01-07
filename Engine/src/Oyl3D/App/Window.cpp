#include "oylpch.h"
#include "Window.h"

#include "Rendering/Renderer.h"

#include "Platform/Windows/Win32Window.h"

namespace oyl
{
    Ref<Window> Window::create(const WindowProps& props)
    {
    #if defined(OYL_PLATFORM_WINDOWS)
        return Ref<Window>(new Win32Window(props));
    #else
        return nullptr;
    #endif
    }
}
