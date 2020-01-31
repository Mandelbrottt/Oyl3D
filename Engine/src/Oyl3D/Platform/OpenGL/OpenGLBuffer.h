#pragma once

#include "Oyl3D/Graphics/Buffer.h"

namespace oyl
{

// Vertex Buffer //////////////////////////////////////////////////

    class OpenGLVertexBuffer : public VertexBuffer
    {
        struct _OpenGLVertexBuffer {};

    public:
        explicit OpenGLVertexBuffer(_OpenGLVertexBuffer, float* vertices, uint size);
        virtual ~OpenGLVertexBuffer();

        void load(float* vertices, uint size) override;
        void unload() override;

        void bind() const override;
        void unbind() const override;

        BufferLayout getLayout() const override { return m_layout; }
        void         setLayout(const BufferLayout& layout) override { m_layout = layout; }

        bool isLoaded() const override { return m_loaded; }

    private:
        uint         m_rendererID = 0;
        BufferLayout m_layout;

        bool m_loaded = false;

        friend class VertexBuffer;
    };

// Index Buffer ///////////////////////////////////////////////////

    class OpenGLIndexBuffer : public IndexBuffer
    {
        struct _OpenGLIndexBuffer {};
        
    public:
        explicit OpenGLIndexBuffer(_OpenGLIndexBuffer, uint* indices, uint count);
        virtual ~OpenGLIndexBuffer();

        void load(uint* indices, uint count) override;
        void unload() override;

        void bind() const override;
        void unbind() const override;

        uint getCount() const override { return m_count; }

        bool isLoaded() const override { return m_loaded; }

    private:
        uint m_rendererID = 0;
        uint m_count      = 0;

        bool m_loaded = false;

        friend class IndexBuffer;
    };

// Vertex Array ///////////////////////////////////////////////////

    class OpenGLVertexArray : public VertexArray
    {
        struct _OpenGLVertexArray {};
        
    public:
        explicit OpenGLVertexArray(_OpenGLVertexArray);
        virtual ~OpenGLVertexArray();

        void load() override;
        void unload() override;

        void bind() const override;
        void unbind() const override;

        void addVertexBuffer(const Ref<VertexBuffer>& vbo) override;
        void addIndexBuffer(const Ref<IndexBuffer>& ebo) override;

        const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const override { return m_vertexBuffers; };
        const Ref<IndexBuffer>&               getIndexBuffer() const override { return m_indexBuffer; }

        bool isLoaded() const override { return m_loaded; }

    private:
        uint m_rendererID = 0;

        std::vector<Ref<VertexBuffer>> m_vertexBuffers;
        Ref<IndexBuffer>               m_indexBuffer;

        bool m_loaded = false;

        friend class VertexArray;
    };

// Frame Buffer ///////////////////////////////////////////////////

    class OpenGLFrameBuffer final : public FrameBuffer
    {
        struct _OpenGLFrameBuffer {};
        
    public:
        explicit OpenGLFrameBuffer(_OpenGLFrameBuffer, int numColorAttachments);
        virtual ~OpenGLFrameBuffer();

        void load(uint numColorAttachments) override;
        void unload() override;

        void bind(FrameBufferContext a_context) override;
        void unbind(FrameBufferContext a_context) override;

        void bindColorAttachment(uint index, uint slot = 0) override;
        void unbindColorAttachment(uint index, uint slot = 0) override;

        void initDepthTexture(int width, int height) override;
        void initColorTexture(uint index,
                              int  width, 
                              int  height,
                              TextureFormat format,
                              TextureFilter filter,
                              TextureWrap   wrap) override;

        void updateViewport(int width, int height) override;
        void clear() override;

        void blit(const Ref<FrameBuffer>& other) override;

        uint getDepthHandle() const override { return m_depthAttachmentID; }
        uint getColorHandle(int index) const override { return m_colorAttachmentIDs[index]; }

        uint getWidth(uint index) const override { return m_widths[index]; }
        uint getHeight(uint index) const override { return m_heights[index]; }
        
        bool isLoaded() const override { return m_loaded; }

    private:
        bool checkFBO();

    private:
        uint m_rendererID        = 0;
        uint m_depthAttachmentID = 0;
        uint m_colorAttachmentIDs[maxColorAttachments]{ 0 };
        uint m_bufs[maxColorAttachments]{ 0 };
        uint m_widths[maxColorAttachments]{ 0 };
        uint m_heights[maxColorAttachments]{ 0 };

        TextureFormat m_formats[maxColorAttachments]{ TextureFormat::RGB8 };
        TextureFilter m_filters[maxColorAttachments]{ TextureFilter::Linear };
        TextureWrap   m_wraps[maxColorAttachments]{ TextureWrap::Repeat };

        uint m_numColorAttachments = 0;

        bool m_loaded = false;

        friend class FrameBuffer;
    };
}
