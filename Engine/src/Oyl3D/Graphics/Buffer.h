#pragma once

#include "Texture.h"

namespace oyl
{
    enum class DataType
    {
        Bool,
        Uint, Uint2, Uint3, Uint4,
        Int, Int2, Int3, Int4,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
    };
    
    static uint shaderDataTypeSize(DataType type)
    {
        switch (type)
        {
            case DataType::Bool:   return 1;
            case DataType::Uint:
            case DataType::Float:
            case DataType::Int:    return 4;
            case DataType::Uint2:
            case DataType::Int2:   
            case DataType::Float2: return 4 * 2;
            case DataType::Uint3:
            case DataType::Int3:   
            case DataType::Float3: return 4 * 3;
            case DataType::Uint4:
            case DataType::Int4:   return 4 * 4;
            case DataType::Float4: return 4 * 4;
            case DataType::Mat3:   return 4 * 3 * 3;
            case DataType::Mat4:   return 4 * 4 * 4;
        }
        OYL_ASSERT(false, "Unknown Type!");
        return 0;
    }

    struct BufferElement
    {
        BufferElement() = default;

        BufferElement(DataType type, std::string name, bool normalized = false)
            : name(std::move(name)), type(type),
              size(shaderDataTypeSize(type)),
              offset(0), normalized(normalized) {}

        uint getElementCount() const
        {
            switch (type)
            {
                case DataType::Bool:   
                case DataType::Uint:   
                case DataType::Int:    
                case DataType::Float:  return 1;
                case DataType::Uint2: 
                case DataType::Int2:
                case DataType::Float2: return 2;
                case DataType::Uint3: 
                case DataType::Int3:   
                case DataType::Float3: return 3;
                case DataType::Uint4: 
                case DataType::Int4:
                case DataType::Float4: return 4;
                case DataType::Mat3:   return 3 * 3;
                case DataType::Mat4:   return 4 * 4;
            }
            OYL_ASSERT(false, "Unknown Type!");
            return 0;
        }

        std::string name;
        DataType    type;
        u32         size;
        u32         offset;
        bool        normalized;
    };

// Buffer Layout //////////////////////////////////////////////////////////////////////////

    class BufferLayout
    {
    public:
        BufferLayout() = default;
        
        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_elements(elements)
        {
            // Calculate the relative offsets and stride for the layout
            uint offset = 0;
            for (auto& element : m_elements)
            {
                element.offset = offset;
                offset += element.size;
                m_stride += element.size;
            }
        }

        uint getStride() const { return m_stride; }
        
        const std::vector<BufferElement>& getElements() const { return m_elements; }

        std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
        std::vector<BufferElement>::iterator end()   { return m_elements.end(); }

        std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const   { return m_elements.end(); }

    private:
        std::vector<BufferElement> m_elements;
        uint                       m_stride = 0;
    };

// Vertex Buffer //////////////////////////////////////////////////////////////////////////

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void load(float* vertices, uint size) = 0;
        virtual void unload() = 0;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual BufferLayout getLayout() const = 0;
        virtual void setLayout(const BufferLayout& layout) = 0;

        virtual bool isLoaded() const = 0;

        static Ref<VertexBuffer> create(float* vertices, uint size);
    };

// Index Buffer ///////////////////////////////////////////////////////////////////////////

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void load(uint* indices, uint count) = 0;
        virtual void unload() = 0;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual uint getCount() const = 0;

        virtual bool isLoaded() const = 0;

        static Ref<IndexBuffer> create(uint* indices, uint size);
    };

// Vertex Array ///////////////////////////////////////////////////////////////////////////

    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void load() = 0;
        virtual void unload() = 0;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        // TODO: Add remove and reset functions for buffers
        virtual void addVertexBuffer(const Ref<VertexBuffer>& vbo) = 0;
        virtual void addIndexBuffer(const Ref<IndexBuffer>& ebo) = 0;

        virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>&               getIndexBuffer() const = 0;

        virtual bool isLoaded() const = 0;

        static Ref<VertexArray> create();
    };

// FrameBuffer ////////////////////////////////////////////////////////////////////////////

    class FrameBuffer
    {
    public:
        virtual ~FrameBuffer() = default;

        virtual void load(uint numColorAttachments) = 0;
        virtual void unload() = 0;

        virtual void bind() = 0;
        virtual void unbind() = 0;

        virtual void initDepthTexture(int width, int height) = 0;
        virtual void initColorTexture(uint index,
                                      int  width, 
                                      int  height,
                                      TextureFormat format,
                                      TextureFilter filter,
                                      TextureWrap   wrap) = 0;

        virtual void updateViewport(int width, int height) = 0;
        virtual void clear() = 0;

        virtual void moveToBackBuffer(int width, int height) = 0;

        virtual uint getDepthHandle() const = 0;
        virtual uint getColorHandle(int index) const = 0;

        virtual bool isLoaded() const = 0;

        static Ref<FrameBuffer> create(int numColorAttachments = 1);

        static const int maxColorAttachments = 8;
    };
}
