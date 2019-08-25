#pragma once

#include "Oyl3D/Graphics/Buffer.h"

namespace oyl {

// Vertex Buffer //////////////////////////////////////////////////

class OpenGLVertexBuffer : public VertexBuffer {
public:
	virtual ~OpenGLVertexBuffer();

	virtual void load(float* vertices, uint size) override;
	virtual void unload() override;

	virtual void bind() const override;
	virtual void unbind() const override;

	virtual BufferLayout getLayout() const override { return m_layout; }
	virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }

	virtual bool isLoaded() const override { return m_loaded; }
private:
	friend class VertexBuffer;
	OpenGLVertexBuffer(float* vertices, uint size);

	uint m_rendererID = 0;
	BufferLayout m_layout;

	bool m_loaded = false;
};

// Index Buffer ///////////////////////////////////////////////////

class OpenGLIndexBuffer : public IndexBuffer {
public:
	virtual ~OpenGLIndexBuffer();

	virtual void load(uint* indices, uint count) override;
	virtual void unload() override;

	virtual void bind() const override;
	virtual void unbind() const override;

	virtual uint getCount() const override { return m_count; }

	virtual bool isLoaded() const override { return m_loaded; }
private:
	friend class IndexBuffer;
	OpenGLIndexBuffer(uint* indices, uint count);

	uint m_rendererID = 0;

	uint m_count = 0;

	bool m_loaded = false;
};

// Vertex Array ///////////////////////////////////////////////////

class OpenGLVertexArray : public VertexArray {
public:
	virtual ~OpenGLVertexArray();

	virtual void load() override;
	virtual void unload() override;

	virtual void bind() const override;
	virtual void unbind() const override;

	virtual void addVertexBuffer(const Ref<VertexBuffer>& vbo) override;
	virtual void addIndexBuffer(const Ref<IndexBuffer>& ebo) override;

	virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const { return m_vertexBuffers; };
	virtual const Ref<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }

	virtual bool isLoaded() const override { return m_loaded; }
private:
	friend class VertexArray;
	OpenGLVertexArray();

	uint m_rendererID = 0;
	std::vector<Ref<VertexBuffer>> m_vertexBuffers;
	Ref<IndexBuffer> m_indexBuffer;

	bool m_loaded = false;
};

// Frame Buffer ///////////////////////////////////////////////////

class OpenGLFrameBuffer : public FrameBuffer {
public:
	virtual ~OpenGLFrameBuffer();

	virtual void load(uint numColorAttachments) override;
	virtual void unload() override;

	virtual void bind() override;
	virtual void unbind() override;

	virtual void initDepthTexture(int width, int height) override;
	virtual void initColorTexture(uint index, 
								  int width, int height, 
								  OylEnum format,
								  OylEnum filter,
								  OylEnum wrap) override;

	virtual void updateViewport(int width, int height) override;
	virtual void clear() override;

	virtual void moveToBackBuffer(int width, int height) override;

	virtual uint getDepthHandle() const { return m_depthAttachmentID; }
	virtual uint getColorHandle(int index) const { return m_colorAttachmentIDs[index]; }

	virtual bool isLoaded() const override { return m_loaded; }
private:
	friend class FrameBuffer;
	OpenGLFrameBuffer(int numColorAttachments);

	bool checkFBO();
private:
	uint m_rendererID = 0;
	uint m_depthAttachmentID = 0;
	uint m_colorAttachmentIDs[FrameBuffer::maxColorAttachments]{ 0 };
	uint m_bufs[FrameBuffer::maxColorAttachments]{ 0 };

	OylEnum m_formats[FrameBuffer::maxColorAttachments];
	OylEnum m_filters[FrameBuffer::maxColorAttachments];
	OylEnum m_wraps[FrameBuffer::maxColorAttachments];

	uint m_numColorAttachments = 0;

	bool m_loaded = false;
};

}