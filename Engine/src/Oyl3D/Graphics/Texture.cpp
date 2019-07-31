#include "oylpch.h"
#include "Texture.h"

#include "Rendering/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace oyl {

Texture1D* Texture1D::create(const std::string& filename) {
	switch (Renderer::getAPI()) {
	case None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return new OpenGLTexture1D(filename);
	}
	return nullptr;
}

Texture2D* Texture2D::create(const std::string& filename) {
	switch (Renderer::getAPI()) {
	case None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return new OpenGLTexture2D(filename);
	}
	return nullptr;
}

Texture3D* Texture3D::create(const std::string& filename) {
	switch (Renderer::getAPI()) {
	case None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return new OpenGLTexture3D(filename);
	}
	return nullptr;
}

}

