#include "oylpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

#include "GLCommon.h"

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

void OpenGLTexture1D::bind() {

}

void OpenGLTexture1D::unbind() {

}

OpenGLTexture2D::OpenGLTexture2D(const std::string& filename) {
	load(filename);
}

OpenGLTexture2D::~OpenGLTexture2D() {
	unload();
}

void OpenGLTexture2D::load(const std::string& filename) {
	GLCall(glGenTextures(1, &m_rendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	} else {
		LOG_ERROR("Texture {0} failed to load!", filename);
		unsigned char* err = new unsigned char[3]{ 255, 0, 255 };
		GLCall(glTexImage2D(GL_TEXTURE, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, err));
	}
	stbi_image_free(data);

	GLCall(glBindTexture(GL_TEXTURE_2D, GL_NONE));
}

void OpenGLTexture2D::unload() {

}

void OpenGLTexture2D::bind() {
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererID));
}

void OpenGLTexture2D::unbind() {
	GLCall(glBindTexture(GL_TEXTURE_2D, GL_NONE));
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

void OpenGLTexture3D::bind() {

}

void OpenGLTexture3D::unbind() {

}

}

