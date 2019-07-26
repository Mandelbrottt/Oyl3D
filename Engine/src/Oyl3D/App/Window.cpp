#include "oylpch.h"
#include "Window.h"

#include "Rendering/Renderer.h"

#include "Platform/Windows/Win32Window.h"


namespace oyl {

Window* Window::create(const WindowProps& props) {
	switch (Renderer::getAPI()) {
	case RendererAPI::API::None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case RendererAPI::API::OpenGL: return new Win32Window(props);
	}
	return nullptr;
}

}