#include "oylpch.h"
#include "OpenGLBuffer.h"

#include "GLCommon.h"

#include <glad/glad.h>

namespace oyl {

// VertexBuffer //////////////////////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint size) {
	load(vertices, size);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
	unload();
}

void OpenGLVertexBuffer::load(float* vertices, uint size) {
	if (m_loaded) return;
	m_loaded = true;

	GLCall(glGenBuffers(1, &m_rendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));

	// Upload data to the GPU
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
}

void OpenGLVertexBuffer::unload() {
	if (!m_loaded) return;
	m_loaded = false;

	GLCall(glDeleteBuffers(1, &m_rendererID));
}

void OpenGLVertexBuffer::bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
}

void OpenGLVertexBuffer::unbind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, GL_NONE));
}

// IndexBuffer ///////////////////////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(uint* indices, uint count)
	: m_count(count) {
	load(indices, count);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
	unload();
}

void OpenGLIndexBuffer::load(uint* indices, uint count){
	if (m_loaded) return;
	m_loaded = true;

	GLCall(glGenBuffers(1, &m_rendererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));

	// Upload data to the GPU
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), indices, GL_STATIC_DRAW));
}

void OpenGLIndexBuffer::unload(){
	if (!m_loaded) return;
	m_loaded = false;

	GLCall(glDeleteBuffers(1, &m_rendererID));
}

void OpenGLIndexBuffer::bind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));
}

void OpenGLIndexBuffer::unbind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE));
}

// Vertex Array //////////////////////////////////////////////////////////////

OpenGLVertexArray::OpenGLVertexArray() {
	load();
}

OpenGLVertexArray::~OpenGLVertexArray() {
	unload();
}

void OpenGLVertexArray::load(){
	if (m_loaded) return;
	m_loaded = true;

	GLCall(glGenVertexArrays(1, &m_rendererID));
}

void OpenGLVertexArray::unload(){
	if (!m_loaded) return;
	m_loaded = false;

	GLCall(glDeleteVertexArrays(1, &m_rendererID));
}

void OpenGLVertexArray::bind() const {
	GLCall(glBindVertexArray(m_rendererID));
}

void OpenGLVertexArray::unbind() const {
	GLCall(glBindVertexArray(GL_NONE));
}

static GLenum ShaderDataTypeToGLType(OylEnum type);

void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) {
	ASSERT(!vbo->getLayout().getElements().empty(), "Layout is Empty!");

	GLCall(glBindVertexArray(m_rendererID));
	vbo->bind();

	// Setup the vertex buffer's layout
	uint index = 0;
	const auto& layout = vbo->getLayout();
	for (const auto& element : vbo->getLayout()) {
		GLCall(glEnableVertexAttribArray(index));
		GLCall(glVertexAttribPointer(index,
							  element.getElementCount(),
							  ShaderDataTypeToGLType(element.type),
							  element.normalized ? GL_TRUE : GL_FALSE,
							  layout.getStride(),
							  (const void*) element.offset));
		index++;
	}

	m_vertexBuffers.push_back(vbo);
	GLCall(glBindVertexArray(GL_NONE));
}

void OpenGLVertexArray::addIndexBuffer(const std::shared_ptr<IndexBuffer>& ebo) {
	GLCall(glBindVertexArray(m_rendererID));
	ebo->bind();

	m_indexBuffer = ebo;
	GLCall(glBindVertexArray(GL_NONE));
}

static GLenum ShaderDataTypeToGLType(OylEnum type) {
	switch (type) {
	case UInt:		return GL_UNSIGNED_INT;
	case Int:		return GL_INT;
	case Int2:		return GL_INT;
	case Int3:		return GL_INT;
	case Int4:		return GL_INT;
	case Float:		return GL_FLOAT;
	case Float2:	return GL_FLOAT;
	case Float3:	return GL_FLOAT;
	case Float4:	return GL_FLOAT;
	case Mat3:		return GL_FLOAT;
	case Mat4:		return GL_FLOAT;
	case Bool:		return GL_BOOL;
	}
	ASSERT(false, "Unknown Type!");
	return 0;
}

// Frame Buffer //////////////////////////////////////////////////////////////

static uint TextureFormatToGLFormat(OylEnum format) {
	switch (format) {
	case RGB8: return GL_RGB8;
	case RGBA8: return GL_RGBA8;
	}
	return 0;
}

static uint TextureFilterToGLFilter(OylEnum filter) {
	switch (filter) {
	case Linear:  return GL_LINEAR; 
	case Nearest: return GL_NEAREST;
	}
	return 0;
}

static uint TextureWrapToGLWrap(OylEnum wrap) {
	switch (wrap) {
	case Clamp:  return GL_CLAMP_TO_EDGE;
	case Mirror: return GL_MIRRORED_REPEAT;
	case Repeat: return GL_REPEAT;
	}
	return 0;
}

OpenGLFrameBuffer::OpenGLFrameBuffer(int numColorAttachments) {
	load(numColorAttachments);
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
	unload();
}

void OpenGLFrameBuffer::load(uint numColorAttachments) {
	if (m_loaded) return;
	m_loaded = true;

	GLCall(glGenFramebuffers(1, &m_rendererID));

	// m_bufs is required as a paramater for glDrawBuffers()
	for (int i = 0; i < numColorAttachments; i++) {
		m_bufs[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	m_numColorAttachments = numColorAttachments;
}

void OpenGLFrameBuffer::unload() {
	if (!m_loaded) return;
	m_loaded = false;

	if (m_bufs[0] != 0) {
		for (auto& buf : m_bufs) buf = 0;
	}

	if (m_colorAttachmentIDs[0] != 0) {
		GLCall(glDeleteTextures(m_numColorAttachments, m_colorAttachmentIDs));

		for (auto& id : m_colorAttachmentIDs) id = 0;
	}

	if (m_depthAttachmentID != GL_NONE) {
		GLCall(glDeleteTextures(1, &m_depthAttachmentID));
		m_depthAttachmentID = GL_NONE;
	}

	m_numColorAttachments = 0;

	GLCall(glDeleteFramebuffers(1, &m_rendererID));
}

void OpenGLFrameBuffer::bind() {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID));
	GLCall(glDrawBuffers(m_numColorAttachments, m_bufs));
}

void OpenGLFrameBuffer::unbind() {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));
}

void OpenGLFrameBuffer::initDepthTexture(int width, int height) {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID));
	
	// Create depth texture
	GLCall(glGenTextures(1, &m_depthAttachmentID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_depthAttachmentID));
	GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// Bind texture to FBO
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachmentID, 0));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));
}

void OpenGLFrameBuffer::initColorTexture(uint index, 
										 int width, int height, 
										 OylEnum format, 
										 OylEnum filter,
										 OylEnum wrap) {
	ASSERT(index < m_numColorAttachments, "Invalid index!");

	m_formats[index] = format;
	m_filters[index] = filter;
	m_wraps[index] = wrap;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID));

	// Create depth texture
	GLCall(glGenTextures(1, &m_colorAttachmentIDs[index]));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_colorAttachmentIDs[index]));
	GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, TextureFormatToGLFormat(format), width, height));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToGLFilter(filter)));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToGLFilter(filter)));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGLWrap(wrap)));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGLWrap(wrap)));

	// Bind texture to FBO
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_colorAttachmentIDs[index], 0));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));
}


void OpenGLFrameBuffer::updateViewport(int width, int height) {
	uint tempNumColorAttachments = m_numColorAttachments;

	unload();
	load(tempNumColorAttachments);

	initDepthTexture(width, height);
	for (int i = 0; i < tempNumColorAttachments; i++) {
		initColorTexture(i, width, height, m_formats[i], m_filters[i], m_wraps[i]);
	}
}

void OpenGLFrameBuffer::clear() {
	GLbitfield temp = 0;

	if (m_depthAttachmentID != GL_NONE) temp |= GL_DEPTH_BUFFER_BIT;

	if (m_numColorAttachments > 0) temp |= GL_COLOR_BUFFER_BIT;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID));
	GLCall(glClear(temp));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));
}

void OpenGLFrameBuffer::moveToBackBuffer(int width, int height) {
	GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID));
	GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE));

	GLCall(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));
}

bool OpenGLFrameBuffer::checkFBO() {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID));

	GLenum status;
	GLCall(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		unload();
		return false;
	}
	return true;
}

}

