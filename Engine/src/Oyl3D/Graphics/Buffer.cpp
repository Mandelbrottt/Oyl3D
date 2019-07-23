#include "oylpch.h"
#include "Buffer.h"

#include "Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace oyl {

oyl::VertexBuffer* VertexBuffer::create(float* vertices, size_t size) {
	switch (Renderer::getAPI()) {
	case RendererAPI::API::None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
	}
	return nullptr;
}

oyl::IndexBuffer* IndexBuffer::create(uint* indices, size_t size) {
	switch (Renderer::getAPI()) {
	case RendererAPI::API::None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
	}
	return nullptr;
}

oyl::VertexArray* VertexArray::create() {
	switch (Renderer::getAPI()) {
	case RendererAPI::API::None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
	}
	return nullptr;
}

}

