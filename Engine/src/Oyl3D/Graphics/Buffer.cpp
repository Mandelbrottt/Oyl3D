#include "oylpch.h"
#include "Buffer.h"

#include "Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace oyl {

oyl::VertexBuffer* VertexBuffer::create(float* vertices, uint size) {
	switch (Renderer::getAPI()) {
	case None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return new OpenGLVertexBuffer(vertices, size);
	}
	return nullptr;
}

oyl::IndexBuffer* IndexBuffer::create(uint* indices, uint size) {
	switch (Renderer::getAPI()) {
	case None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return new OpenGLIndexBuffer(indices, size);
	}
	return nullptr;
}

oyl::VertexArray* VertexArray::create() {
	switch (Renderer::getAPI()) {
	case None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return new OpenGLVertexArray();
	}
	return nullptr;
}

oyl::FrameBuffer* FrameBuffer::create(int numColorAttachments) {
	switch (Renderer::getAPI()) {
	case None: ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return new OpenGLFrameBuffer(numColorAttachments);
	}
	return nullptr;
}

}
