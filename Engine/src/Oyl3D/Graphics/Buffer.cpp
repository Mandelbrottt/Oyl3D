#include "oylpch.h"
#include "Buffer.h"

#include "Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace oyl {

Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint size) {
	switch (Renderer::getAPI()) {
	case None: OYL_ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return Ref<VertexBuffer>(new OpenGLVertexBuffer(vertices, size));
	}
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::create(uint* indices, uint size) {
	switch (Renderer::getAPI()) {
	case None: OYL_ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return Ref<IndexBuffer>(new OpenGLIndexBuffer(indices, size));
	}
	return nullptr;
}

Ref<VertexArray> VertexArray::create() {
	switch (Renderer::getAPI()) {
	case None: OYL_ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return Ref<VertexArray>(new OpenGLVertexArray());
	}
	return nullptr;
}

Ref<FrameBuffer> FrameBuffer::create(int numColorAttachments) {
	switch (Renderer::getAPI()) {
	case None: OYL_ASSERT(false, "None is currently unsupported"); return nullptr;
	case API_OpenGL: return Ref<FrameBuffer>(new OpenGLFrameBuffer(numColorAttachments));
	}
	return nullptr;
}

}

