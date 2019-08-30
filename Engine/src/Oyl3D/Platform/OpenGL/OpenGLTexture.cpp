#include "oylpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace oyl {

OpenGLTexture1D::OpenGLTexture1D(const std::string& filename) {
	load(filename);
}

OpenGLTexture1D::~OpenGLTexture1D() {
	unload();
}

void OpenGLTexture1D::load(const std::string& filename) {

}

void OpenGLTexture1D::unload() {

}

void OpenGLTexture1D::bind(uint slot) const {

}

void OpenGLTexture1D::unbind() const {

}

OpenGLTexture2D::OpenGLTexture2D(const std::string& filename) {
	load(filename);
}

OpenGLTexture2D::~OpenGLTexture2D() {
	unload();
}

void OpenGLTexture2D::load(const std::string& filename) {
	if (m_loaded) return;

	glGenTextures(1, &m_rendererID);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	u8* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		m_loaded = true;
		m_width = width;
		m_height = height;
		m_path = filename;

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		OYL_LOG_ERROR("Texture {0} failed to load!", filename);
		u8* err = new u8[3]{ 0xFF, 0, 0xFF };

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1, 1);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGB8, GL_UNSIGNED_BYTE, err);
	}
	stbi_image_free(data);
}

void OpenGLTexture2D::unload() {
	if (!m_loaded) return;
	m_loaded = false;

	glDeleteTextures(1, &m_rendererID);
}

void OpenGLTexture2D::bind(uint slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void OpenGLTexture2D::unbind() const {
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

OpenGLTexture3D::OpenGLTexture3D(const std::string& filename) {
	load(filename);
}

OpenGLTexture3D::~OpenGLTexture3D() {
	unload();
}

void OpenGLTexture3D::load(const std::string& filename) {

}

void OpenGLTexture3D::unload() {

}

void OpenGLTexture3D::bind(uint slot) const {

}

void OpenGLTexture3D::unbind() const {

}

}

