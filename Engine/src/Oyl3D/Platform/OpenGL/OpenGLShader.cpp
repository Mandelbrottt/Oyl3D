#include "oylpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace oyl {

static GLuint compileShader(GLuint type, const std::string& src) {
	if (src.empty()) return 0;

	GLuint shader = glCreateShader(type);

	const char* source = src.c_str();
	glShaderSource(shader, 1, &source, 0);

	glCompileShader(shader);

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		glDeleteShader(shader);

		LOG_ERROR("{0}", infoLog.data());
		std::string err = " shader compilation failure!";
		switch (type) {
		case GL_VERTEX_SHADER: err.insert(0, "Vertex"); break;
		case GL_GEOMETRY_SHADER: err.insert(0, "Geometry"); break;
		case GL_FRAGMENT_SHADER: err.insert(0, "Fragment"); break;
		}
		ASSERT(false, err);

		return 0;
	}
	return shader;
}

static void linkShaders(const uint id, const GLuint vertShader, const GLuint geomShader, const GLuint fragShader) {
	if (vertShader != 0) glAttachShader(id, vertShader);
	if (geomShader != 0) glAttachShader(id, geomShader);
	if (fragShader != 0) glAttachShader(id, fragShader);

	glLinkProgram(id);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(id, GL_LINK_STATUS, (int*) & isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(id, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(id);
		// Don't leak shaders either.
		if (vertShader != 0) glDeleteShader(vertShader);
		if (geomShader != 0) glDeleteShader(geomShader);
		if (fragShader != 0) glDeleteShader(fragShader);

		LOG_ERROR("{0}", infoLog.data());
		ASSERT(false, "Shader link failure!");

		return;
	}

	// Always detach shaders after a successful link.
	if (vertShader != 0) glDetachShader(id, vertShader);
	if (geomShader != 0) glDetachShader(id, geomShader);
	if (fragShader != 0) glDetachShader(id, fragShader);
}

void OpenGLShader::processShaders(const std::string& vertSrc, const std::string& geomSrc, const std::string& fragSrc) {
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertSrc);
	
	GLuint geometryShader = compileShader(GL_GEOMETRY_SHADER, geomSrc);
	
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragSrc);

	m_rendererID = glCreateProgram();

	linkShaders(m_rendererID, vertexShader, geometryShader, fragmentShader);
}

OpenGLShader::OpenGLShader(const std::string& vertSrc, const std::string& fragSrc) {
	processShaders(vertSrc, "", fragSrc);
}

OpenGLShader::OpenGLShader(const std::initializer_list<ShaderInfo>& files) {
	std::vector<ShaderInfo> infos(files);
	
	std::string srcs[3]{ "" };
	for (auto& info : infos) {
		ASSERT(srcs[(int) info.type - 1].empty(), "Multiple same type shaders defined!");
		std::ifstream i(info.filename);
		ASSERT(i, "File could not open!");
		std::stringstream ss;
		ss << i.rdbuf();
		srcs[(int) info.type - 1] = ss.str();
	}

	processShaders(srcs[0], srcs[1], srcs[2]);
}

OpenGLShader::~OpenGLShader() {
	glDeleteProgram(m_rendererID);
}

void OpenGLShader::bind() const {
	glUseProgram(m_rendererID);
}

void OpenGLShader::unbind() const {
	glUseProgram(0);
}

}

