#include "oylpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace oyl {

// VertexBuffer //////////////////////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint size) {
	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);

	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
	glDeleteBuffers(1, &m_rendererID);
}

void OpenGLVertexBuffer::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void OpenGLVertexBuffer::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// IndexBuffer ///////////////////////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(uint* indices, uint count)
	: m_count(count) {
	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
	glDeleteBuffers(1, &m_rendererID);
}

void OpenGLIndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void OpenGLIndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Vertex Array //////////////////////////////////////////////////////////////

OpenGLVertexArray::OpenGLVertexArray() {
	glGenVertexArrays(1, &m_rendererID);
}

void OpenGLVertexArray::bind() const {
	glBindVertexArray(m_rendererID);
}

void OpenGLVertexArray::unbind() const{
	glBindVertexArray(0);
}

static GLenum ShaderDataTypeToGLType(ShaderDataType type);

void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) {
	ASSERT(!vbo->getLayout().getElements().empty(), "Layout is Empty!");

	glBindVertexArray(m_rendererID);
	vbo->bind();

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
	glBindVertexArray(0);
}

void OpenGLVertexArray::addIndexBuffer(const std::shared_ptr<IndexBuffer>& ebo) {
	glBindVertexArray(m_rendererID);
	ebo->bind();

	m_indexBuffer = ebo;
	glBindVertexArray(0);
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

}

