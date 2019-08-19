#pragma once

namespace oyl {

static uint shaderDataTypeSize(OylEnum type) {
	switch (type) {
	case UInt:		return 4;
	case Int:		return 4;
	case Int2:		return 4 * 2;
	case Int3:		return 4 * 3;
	case Int4:		return 4 * 4;
	case Float:		return 4;
	case Float2:	return 4 * 2;
	case Float3:	return 4 * 3;
	case Float4:	return 4 * 4;
	case Mat3:		return 4 * 3 * 3;
	case Mat4:		return 4 * 4 * 4;
	case Bool:		return 1;
	}

	ASSERT(false, "Unknown Type!");
	return 0;
}

struct BufferElement {
	BufferElement() {}

	BufferElement(OylEnum type, const std::string& name, bool normalized = false)
		: name(name), type(type), size(shaderDataTypeSize(type)), offset(0), normalized(normalized) {}

	uint getElementCount() const {
		switch (type) {
		case UInt:		return 1;
		case Int:		return 1;
		case Int2:		return 2;
		case Int3:		return 3;
		case Int4:		return 4;
		case Float:		return 1;
		case Float2:	return 2;
		case Float3:	return 3;
		case Float4:	return 4;
		case Mat3:		return 3 * 3;
		case Mat4:		return 4 * 4;
		case Bool:		return 1;
		}
		ASSERT(false, "Unknown Type!");
		return 0;
	}

	std::string name;
	OylEnum type;
	uint size;
	uint offset;
	bool normalized;
};

// Buffer Layout //////////////////////////////////////////////////////////////////////////

class BufferLayout {
public:
	BufferLayout() {}

	BufferLayout(const std::initializer_list<BufferElement>& elements) 
		: m_elements(elements) {
		// Calculate the relative offsets and stride for the layout
		uint offset = 0;
		for (auto& element : m_elements) {
			element.offset = offset;
			offset += element.size;
			m_stride += element.size;
		}
	}

	inline uint getStride() const { return m_stride; }
	inline const std::vector<BufferElement>& getElements() const { return m_elements; }

	std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_elements.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }
private:
	std::vector<BufferElement> m_elements;
	uint m_stride = 0;
};

// Vertex Buffer //////////////////////////////////////////////////////////////////////////

class VertexBuffer {
public:
	virtual ~VertexBuffer() {}

	virtual void load(float* vertices, uint size) = 0;
	virtual void unload() = 0;
	
	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual BufferLayout getLayout() const = 0;
	virtual void setLayout(const BufferLayout& layout) = 0;

	virtual bool isLoaded() const = 0;

	static VertexBuffer* create(float* vertices, uint size);
};

// Index Buffer ///////////////////////////////////////////////////////////////////////////

class IndexBuffer {
public:
	virtual ~IndexBuffer() {}

	virtual void load(uint* indices, uint count) = 0;
	virtual void unload() = 0;

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual uint getCount() const = 0;

	virtual bool isLoaded() const = 0;

	static IndexBuffer* create(uint* indices, uint size);
};

// Vertex Array ///////////////////////////////////////////////////////////////////////////

class VertexArray {
public:
	virtual ~VertexArray() {}

	virtual void load() = 0;
	virtual void unload() = 0;

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual void addVertexBuffer(const ref<VertexBuffer>& vbo) = 0;
	virtual void addIndexBuffer(const ref<IndexBuffer>& ebo) = 0;

	virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers() const = 0;
	virtual const ref<IndexBuffer>& getIndexBuffer() const = 0;
	
	virtual bool isLoaded() const = 0;

	static VertexArray* create();
};

// FrameBuffer ////////////////////////////////////////////////////////////////////////////

class FrameBuffer {
public:
	virtual ~FrameBuffer() {}

	virtual void load(uint numColorAttachments) = 0;
	virtual void unload() = 0;

	virtual void bind() = 0;
	virtual void unbind() = 0;

	virtual void initDepthTexture(int width, int height) = 0;
	virtual void initColorTexture(uint index, 
								  int width, int height, 
								  OylEnum format, 
								  OylEnum filter,
								  OylEnum wrap) = 0;

	virtual void updateViewport(int width, int height) = 0;
	virtual void clear() = 0;

	virtual void moveToBackBuffer(int width, int height) = 0;

	virtual uint getDepthHandle() const = 0;
	virtual uint getColorHandle(int index) const = 0;

	virtual bool isLoaded() const = 0;

	static FrameBuffer* create(int numColorAttachments = 1);

	static const int maxColorAttachments = 8;

};

}