#include "oylpch.h"
#include "Buffer.h"

#include "Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace oyl
{
    Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint size)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                return Ref<OpenGLVertexBuffer>::create(OpenGLVertexBuffer::_OpenGLVertexBuffer{}, 
                                                       vertices, size);
        }
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::create(uint* indices, uint size)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                return Ref<OpenGLIndexBuffer>::create(OpenGLIndexBuffer::_OpenGLIndexBuffer{},
                                                      indices, size);
        }
        return nullptr;
    }

    Ref<VertexArray> VertexArray::create()
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                return Ref<OpenGLVertexArray>::create(OpenGLVertexArray::_OpenGLVertexArray{});
        }
        return nullptr;
    }

    Ref<FrameBuffer> FrameBuffer::create(int numColorAttachments)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                return Ref<OpenGLFrameBuffer>::create(OpenGLFrameBuffer::_OpenGLFrameBuffer{},
                                                      numColorAttachments);
        }
        return nullptr;
    }
}
