#include "oylpch.h"
#include "GraphicsContext.h"

#include "Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace oyl {

GraphicsContext* GraphicsContext::create(void* window) {
	switch (Renderer::getAPI()) {
	case None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return new OpenGLContext((GLFWwindow*) window);
	}
}

}


