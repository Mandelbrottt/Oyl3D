#include "oylpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace oyl
{

// VertexBuffer //////////////////////////////////////////////////////////////

    OpenGLVertexBuffer::OpenGLVertexBuffer(_OpenGLVertexBuffer, float* vertices, uint size)
    {
        OpenGLVertexBuffer::load(vertices, size);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        OpenGLVertexBuffer::unload();
    }

    void OpenGLVertexBuffer::load(float* vertices, uint size)
    {
        if (m_loaded) return;
        m_loaded = true;

        glGenBuffers(1, &m_rendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);

        // Upload data to the GPU
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    void OpenGLVertexBuffer::unload()
    {
        if (!m_loaded) return;
        m_loaded = false;

        glDeleteBuffers(1, &m_rendererID);
    }

    void OpenGLVertexBuffer::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    }

    void OpenGLVertexBuffer::unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    }

// IndexBuffer ///////////////////////////////////////////////////////////////

    OpenGLIndexBuffer::OpenGLIndexBuffer(_OpenGLIndexBuffer, uint* indices, uint count)
        : m_count(count)
    {
        OpenGLIndexBuffer::load(indices, count);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        OpenGLIndexBuffer::unload();
    }

    void OpenGLIndexBuffer::load(uint* indices, uint count)
    {
        if (m_loaded) return;
        m_loaded = true;

        glGenBuffers(1, &m_rendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);

        // Upload data to the GPU
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), indices, GL_STATIC_DRAW);
    }

    void OpenGLIndexBuffer::unload()
    {
        if (!m_loaded) return;
        m_loaded = false;

        glDeleteBuffers(1, &m_rendererID);
    }

    void OpenGLIndexBuffer::bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    }

    void OpenGLIndexBuffer::unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
    }

// Vertex Array //////////////////////////////////////////////////////////////

    OpenGLVertexArray::OpenGLVertexArray(_OpenGLVertexArray)
    {
        load();
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        unload();
    }

    void OpenGLVertexArray::load()
    {
        if (m_loaded) unload();
        m_loaded = true;

        glGenVertexArrays(1, &m_rendererID);
    }

    void OpenGLVertexArray::unload()
    {
        if (!m_loaded) return;
        m_loaded = false;

        glDeleteVertexArrays(1, &m_rendererID);
        m_vertexBuffers.clear();
        m_indexBuffer.reset();
    }

    void OpenGLVertexArray::bind() const
    {
        glBindVertexArray(m_rendererID);
    }

    void OpenGLVertexArray::unbind() const
    {
        glBindVertexArray(GL_NONE);
    }

    static GLenum ShaderDataTypeToGLType(DataType type);

    void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vbo)
    {
        OYL_ASSERT(!vbo->getLayout().getElements().empty(), "Layout is Empty!");

        glBindVertexArray(m_rendererID);

        u32 index  = 0;

        for (const auto& tVbo : m_vertexBuffers)
            index += tVbo->getLayout().getElements().size();

        vbo->bind();

        const auto& layout = vbo->getLayout();
        for (const auto& element : layout)
        {
            switch (element.type)
            {
                case DataType::Bool:
                case DataType::Int:
                case DataType::Int2:
                case DataType::Int3:
                case DataType::Int4:
                case DataType::Uint:
                case DataType::Uint2:
                case DataType::Uint3:
                case DataType::Uint4:
                    if (!element.normalized)
                    {
                        glEnableVertexAttribArray(index);
                        glVertexAttribIPointer(index,
                                               element.getElementCount(),
                                               ShaderDataTypeToGLType(element.type),
                                               layout.getStride(),
                                               (const void*) element.offset);
                        break;
                    }
                default:
                    glEnableVertexAttribArray(index);
                    glVertexAttribPointer(index,
                                          element.getElementCount(),
                                          ShaderDataTypeToGLType(element.type),
                                          element.normalized ? GL_TRUE : GL_FALSE,
                                          layout.getStride(),
                                          (const void*) element.offset);
                    break;
            }
            index++;
        }
        
        m_vertexBuffers.push_back(vbo);
        glBindVertexArray(GL_NONE);
    }

    void OpenGLVertexArray::addIndexBuffer(const Ref<IndexBuffer>& ebo)
    {
        glBindVertexArray(m_rendererID);
        ebo->bind();

        m_indexBuffer = ebo;
        glBindVertexArray(GL_NONE);
    }

    static GLenum ShaderDataTypeToGLType(DataType type)
    {
        switch (type)
        {
            case DataType::Uint:   return GL_UNSIGNED_INT;
            case DataType::Int:    return GL_INT;
            case DataType::Int2:   return GL_INT;
            case DataType::Int3:   return GL_INT;
            case DataType::Int4:   return GL_INT;
            case DataType::Float:  return GL_FLOAT;
            case DataType::Float2: return GL_FLOAT;
            case DataType::Float3: return GL_FLOAT;
            case DataType::Float4: return GL_FLOAT;
            case DataType::Mat3:   return GL_FLOAT;
            case DataType::Mat4:   return GL_FLOAT;
            case DataType::Bool:   return GL_BOOL;
        }
        OYL_ASSERT(false, "Unknown Type!");
        return 0;
    }

// Frame Buffer //////////////////////////////////////////////////////////////

    static uint TextureFormatToGLFormat(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::R8:      return GL_R8;
            case TextureFormat::RG8:     return GL_RG8;
            case TextureFormat::RGB8:    return GL_RGB8;
            case TextureFormat::RGBA8:   return GL_RGBA8;
            case TextureFormat::RGBF16:  return GL_RGB16F;
            case TextureFormat::RGBF32:  return GL_RGB32F;
            case TextureFormat::RGBAF16: return GL_RGBA16F;
            case TextureFormat::RGBAF32: return GL_RGBA32F;
        }
        return 0;
    }

    static uint TexturefilterToGLFilter(TextureFilter filter)
    {
        switch (filter)
        {
            case TextureFilter::Linear:  return GL_LINEAR;
            case TextureFilter::Nearest: return GL_NEAREST;
        }
        return 0;
    }

    static uint TextureWrapToGLWrap(TextureWrap wrap)
    {
        switch (wrap)
        {
            case TextureWrap::Repeat:         return GL_REPEAT;
            case TextureWrap::Mirror:         return GL_MIRRORED_REPEAT;
            case TextureWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
            case TextureWrap::ClampToBorder:  return GL_CLAMP_TO_BORDER;
        }
        return 0;
    }

    OpenGLFrameBuffer::OpenGLFrameBuffer(_OpenGLFrameBuffer, int numColorAttachments)
    {
        OpenGLFrameBuffer::load(numColorAttachments);
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        OpenGLFrameBuffer::unload();
    }

    void OpenGLFrameBuffer::load(uint numColorAttachments)
    {
        if (m_loaded) return;
        m_loaded = true;

        glGenFramebuffers(1, &m_rendererID);

        // m_bufs is required as a paramater for glDrawBuffers()
        for (int i = 0; i < numColorAttachments; i++)
        {
            m_bufs[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        m_numColorAttachments = numColorAttachments;

        if (numColorAttachments == 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
        }
    }

    void OpenGLFrameBuffer::unload()
    {
        if (!m_loaded) return;
        m_loaded = false;

        if (m_bufs[0] != 0)
        {
            for (auto& buf : m_bufs) buf = 0;
        }

        if (m_colorAttachmentIDs[0] != 0)
        {
            glDeleteTextures(m_numColorAttachments, m_colorAttachmentIDs);

            for (auto& id : m_colorAttachmentIDs) id = 0;
        }

        if (m_depthAttachmentID != GL_NONE)
        {
            glDeleteTextures(1, &m_depthAttachmentID);
            m_depthAttachmentID = GL_NONE;
        }

        m_numColorAttachments = 0;

        glDeleteFramebuffers(1, &m_rendererID);
    }

    void OpenGLFrameBuffer::bind(FrameBufferContext a_context)
    {
        GLenum context = GL_NONE;
        switch (a_context)
        {
            case FrameBufferContext::ReadWrite: context = GL_FRAMEBUFFER; break;
            case FrameBufferContext::Read: context = GL_READ_FRAMEBUFFER; break;
            case FrameBufferContext::Write: context = GL_DRAW_FRAMEBUFFER; break;
        }
        glBindFramebuffer(context, m_rendererID);
        glDrawBuffers(m_numColorAttachments, m_bufs);
    }

    void OpenGLFrameBuffer::unbind(FrameBufferContext a_context)
    {
        GLenum context = GL_NONE;
        switch (a_context)
        {
            case FrameBufferContext::ReadWrite: context = GL_FRAMEBUFFER; break;
            case FrameBufferContext::Read: context = GL_READ_FRAMEBUFFER; break;
            case FrameBufferContext::Write: context = GL_DRAW_FRAMEBUFFER; break;
        }
        glBindFramebuffer(context, GL_NONE);
    }

    void OpenGLFrameBuffer::bindDepthAttachment(uint slot)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_depthAttachmentID);
    }
    
    void OpenGLFrameBuffer::unbindDepthAttachment(uint slot)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    void OpenGLFrameBuffer::bindColorAttachment(uint index, uint slot)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_colorAttachmentIDs[index]);
    }
    
    void OpenGLFrameBuffer::unbindColorAttachment(uint index, uint slot)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    void OpenGLFrameBuffer::initDepthTexture(int width, int height)
    {
        m_depthWidth = width;
        m_depthHeight = height;

        glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

        if (m_depthAttachmentID)
        {
            glDeleteTextures(1, &m_depthAttachmentID);
            m_depthAttachmentID = 0;
        }
	
        // Create depth texture
        glGenTextures(1, &m_depthAttachmentID);
        glBindTexture(GL_TEXTURE_2D, m_depthAttachmentID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // Bind texture to FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachmentID, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    }

    void OpenGLFrameBuffer::initColorTexture(uint index,
                                             int  width, 
                                             int  height,
                                             TextureFormat format,
                                             TextureFilter filter,
                                             TextureWrap   wrap)
    {
        OYL_ASSERT(index < m_numColorAttachments, "Invalid index!");

        m_formats[index] = format;
        m_filters[index] = filter;
        m_wraps[index]   = wrap;
        m_colorWidths[index] = width;
        m_colorHeights[index] = height;

        glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

        // Create depth texture
        glGenTextures(1, &m_colorAttachmentIDs[index]);
        glBindTexture(GL_TEXTURE_2D, m_colorAttachmentIDs[index]);
        glTexStorage2D(GL_TEXTURE_2D, 1, TextureFormatToGLFormat(format), width, height);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TexturefilterToGLFilter(filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TexturefilterToGLFilter(filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGLWrap(wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGLWrap(wrap));

        // Bind texture to FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_colorAttachmentIDs[index], 0);

        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    }

    void OpenGLFrameBuffer::updateViewport(int width, int height)
    {
        uint tempNumColorAttachments = m_numColorAttachments;

        unload();
        load(tempNumColorAttachments);

        initDepthTexture(width, height);
        for (int i = 0; i < tempNumColorAttachments; i++)
        {
            initColorTexture(i, width, height, m_formats[i], m_filters[i], m_wraps[i]);
        }
    }

    void OpenGLFrameBuffer::clear()
    {
        GLbitfield temp = 0;

        if (m_depthAttachmentID != GL_NONE) temp |= GL_DEPTH_BUFFER_BIT;

        if (m_numColorAttachments > 0) temp |= GL_COLOR_BUFFER_BIT;

        glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
        glClear(temp);
    }

    void OpenGLFrameBuffer::blit(const Ref<FrameBuffer>& other)
    {
        OpenGLFrameBuffer* otherPtr = reinterpret_cast<OpenGLFrameBuffer*>(other.get());
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
        if (other)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, otherPtr->m_rendererID);
            glBlitFramebuffer(0, 0, m_colorWidths[0], m_colorHeights[0], 
                              0, 0, otherPtr->m_colorWidths[0], otherPtr->m_colorHeights[0],
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        else
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
            glBlitFramebuffer(0, 0, m_colorWidths[0], m_colorHeights[0], 
                              0, 0, m_colorWidths[0], m_colorHeights[0],
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    }

    bool OpenGLFrameBuffer::checkFBO()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            unload();
            return false;
        }
        return true;
    }
}
