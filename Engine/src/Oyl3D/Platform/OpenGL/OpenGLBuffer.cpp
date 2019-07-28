#include "oylpch.h"
#include "OpenGLBuffer.h"

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

	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);

	// Upload data to the GPU
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void OpenGLVertexBuffer::unload() {
	if (!m_loaded) return;
	m_loaded = false;

	glDeleteBuffers(1, &m_rendererID);
}

void OpenGLVertexBuffer::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void OpenGLVertexBuffer::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
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

	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);

	// Upload data to the GPU
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), indices, GL_STATIC_DRAW);
}

void OpenGLIndexBuffer::unload(){
	if (!m_loaded) return;
	m_loaded = false;

	glDeleteBuffers(1, &m_rendererID);
}

void OpenGLIndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void OpenGLIndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
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

	glGenVertexArrays(1, &m_rendererID);
}

void OpenGLVertexArray::unload(){
	if (!m_loaded) return;
	m_loaded = false;

	glDeleteVertexArrays(1, &m_rendererID);
}

void OpenGLVertexArray::bind() const {
	glBindVertexArray(m_rendererID);
}

void OpenGLVertexArray::unbind() const {
	glBindVertexArray(GL_NONE);
}

static GLenum ShaderDataTypeToGLType(ShaderDataType type);

void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) {
	ASSERT(!vbo->getLayout().getElements().empty(), "Layout is Empty!");

	glBindVertexArray(m_rendererID);
	vbo->bind();

	// Setup the vertex buffer's layout
	uint index = 0;
	const auto& layout = vbo->getLayout();
	for (const auto& element : vbo->getLayout()) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index,
							  element.getElementCount(),
							  ShaderDataTypeToGLType(element.type),
							  element.normalized ? GL_TRUE : GL_FALSE,
							  layout.getStride(),
							  (const void*) element.offset);
		index++;
	}

	m_vertexBuffers.push_back(vbo);
	glBindVertexArray(GL_NONE);
}

void OpenGLVertexArray::addIndexBuffer(const std::shared_ptr<IndexBuffer>& ebo) {
	glBindVertexArray(m_rendererID);
	ebo->bind();

	m_indexBuffer = ebo;
	glBindVertexArray(GL_NONE);
}

static GLenum ShaderDataTypeToGLType(ShaderDataType type) {
	switch (type) {
	case ShaderDataType::UInt:		return GL_UNSIGNED_INT;
	case ShaderDataType::Int:		return GL_INT;
	case ShaderDataType::Int2:		return GL_INT;
	case ShaderDataType::Int3:		return GL_INT;
	case ShaderDataType::Int4:		return GL_INT;
	case ShaderDataType::Float:		return GL_FLOAT;
	case ShaderDataType::Float2:	return GL_FLOAT;
	case ShaderDataType::Float3:	return GL_FLOAT;
	case ShaderDataType::Float4:	return GL_FLOAT;
	case ShaderDataType::Mat3:		return GL_FLOAT;
	case ShaderDataType::Mat4:		return GL_FLOAT;
	case ShaderDataType::Bool:		return GL_BOOL;
	}
	ASSERT(false, "Unknown Type!");
	return 0;
}

// Frame Buffer //////////////////////////////////////////////////////////////

static uint TextureFormatToGLFormat(TextureFormat format) {
	switch (format) {
	case TextureFormat::RGB8: return GL_RGB8;
	case TextureFormat::RGBA8: return GL_RGBA8;
	}
	return 0;
}

static uint TextureFilterToGLFilter(TextureFilter filter) {
	switch (filter) {
	case TextureFilter::Linear:  return GL_LINEAR; 
	case TextureFilter::Nearest: return GL_NEAREST;
	}
	return 0;
}

static uint TextureWrapToGLWrap(TextureWrap wrap) {
	switch (wrap) {
	case TextureWrap::Clamp:  return GL_CLAMP_TO_EDGE;
	case TextureWrap::Mirror: return GL_MIRRORED_REPEAT;
	case TextureWrap::Repeat: return GL_REPEAT;
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

	glGenFramebuffers(1, &m_rendererID);

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
		glDeleteTextures(m_numColorAttachments, m_colorAttachmentIDs);

		for (auto& id : m_colorAttachmentIDs) id = 0;
	}

	if (m_depthAttachmentID != GL_NONE) {
		glDeleteTextures(1, &m_depthAttachmentID);
		m_depthAttachmentID = GL_NONE;
	}

	m_numColorAttachments = 0;

	glDeleteFramebuffers(1, &m_rendererID);
}

void OpenGLFrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	glDrawBuffers(m_numColorAttachments, m_bufs);
}

void OpenGLFrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void OpenGLFrameBuffer::initDepthTexture(int width, int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	
	// Create depth texture
	glGenTextures(1, &m_depthAttachmentID);
	glBindTexture(GL_TEXTURE_2D, m_depthAttachmentID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Bind texture to FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachmentID, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void OpenGLFrameBuffer::initColorTexture(uint index, 
										 int width, int height, 
										 TextureFormat format, 
										 TextureFilter filter, 
										 TextureWrap wrap) {
	ASSERT(index < m_numColorAttachments, "Invalid index!");

	m_formats[index] = format;
	m_filters[index] = filter;
	m_wraps[index] = wrap;

	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

	// Create depth texture
	glGenTextures(1, &m_colorAttachmentIDs[index]);
	glBindTexture(GL_TEXTURE_2D, m_colorAttachmentIDs[index]);
	glTexStorage2D(GL_TEXTURE_2D, 1, TextureFormatToGLFormat(format), width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToGLFilter(filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToGLFilter(filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGLWrap(wrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGLWrap(wrap));

	// Bind texture to FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_colorAttachmentIDs[index], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
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

	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	glClear(temp);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void OpenGLFrameBuffer::moveToBackBuffer(int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

bool OpenGLFrameBuffer::checkFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		unload();
		return false;
	}
	return true;
}

}

