#pragma once

#include "Oyl3D/Graphics/Buffer.h"

namespace oyl {

class OpenGLVertexBuffer : public VertexBuffer {
public:
	OpenGLVertexBuffer(float* vertices, uint size);
	virtual ~OpenGLVertexBuffer();

	virtual void bind() const override;
	virtual void unbind() const override;

	virtual BufferLayout getLayout() const override { return m_layout; }
	virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }
private:
	uint m_rendererID;
	BufferLayout m_layout;
};

class OpenGLIndexBuffer : public IndexBuffer {
public:
	OpenGLIndexBuffer(uint* indices, uint count);
	virtual ~OpenGLIndexBuffer();

	virtual void bind() const override;
	virtual void unbind() const override;

	virtual uint getCount() const override { return m_count; }
private:
	uint m_rendererID;

	uint m_count;
};

class OpenGLVertexArray : public VertexArray {
public:
	OpenGLVertexArray();
	virtual ~OpenGLVertexArray() {}

	virtual void bind() const override;
	virtual void unbind() const override;

	virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) override;
	virtual void addIndexBuffer(const std::shared_ptr<IndexBuffer>& ebo) override;

	virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const { return m_vertexBuffers; };
	virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }
private:
	uint m_rendererID;
	std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
	std::shared_ptr<IndexBuffer> m_indexBuffer;
};

}