#include "oylpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace oyl {

Shader* Shader::create(const std::string& vertexSrc, const std::string& fragmentSrc) {
	switch (Renderer::getAPI()) {
	case RendererAPI::API::None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
	}
	return nullptr;
}

Shader* Shader::create(const std::initializer_list<ShaderInfo>& files) {
	switch (Renderer::getAPI()) {
	case RendererAPI::API::None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case RendererAPI::API::OpenGL: return new OpenGLShader(files);
	}
	return nullptr;
}


}



