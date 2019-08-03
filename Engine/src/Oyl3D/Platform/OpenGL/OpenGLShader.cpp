#include "oylpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace oyl {

static GLuint compileShader(GLuint type, const std::string& src) {
	if (src.empty()) return 0;

	GLuint shader;
	shader = glCreateShader(type);

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
		case GL_VERTEX_SHADER:			err.insert(0, "Vertex"); break;
		case GL_TESS_CONTROL_SHADER:	err.insert(0, "Tesselation Control"); break;
		case GL_TESS_EVALUATION_SHADER: err.insert(0, "Tesselation Evaluation"); break;
		case GL_GEOMETRY_SHADER:		err.insert(0, "Geometry"); break;
		case GL_FRAGMENT_SHADER:		err.insert(0, "Fragment"); break;
		}
		ASSERT(false, err);

		return 0;
	}
	return shader;
}

static void linkShaders(const uint id, 
						const GLuint vertShader, 
						const GLuint tescShader, 
						const GLuint teseShader, 
						const GLuint geomShader, 
						const GLuint fragShader) {

	if (vertShader != 0) glAttachShader(id, vertShader);
	if (tescShader != 0) glAttachShader(id, tescShader);
	if (teseShader != 0) glAttachShader(id, teseShader);
	if (geomShader != 0) glAttachShader(id, geomShader);
	if (fragShader != 0) glAttachShader(id, fragShader);

	glLinkProgram(id);

	GLint isLinked = 0;
	glGetProgramiv(id, GL_LINK_STATUS, (int*) & isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(id, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(id);

		if (vertShader != 0) glDeleteShader(vertShader);
		if (tescShader != 0) glDeleteShader(tescShader);
		if (teseShader != 0) glDeleteShader(teseShader);
		if (geomShader != 0) glDeleteShader(geomShader);
		if (fragShader != 0) glDeleteShader(fragShader);

		LOG_ERROR("{0}", infoLog.data());
		ASSERT(false, "Shader link failure!");

		return;
	}

	// Always detach shaders after a successful link.
	if (vertShader != 0) glDetachShader(id, vertShader);
	if (tescShader != 0) glDetachShader(id, tescShader);
	if (teseShader != 0) glDetachShader(id, teseShader);
	if (geomShader != 0) glDetachShader(id, geomShader);
	if (fragShader != 0) glDetachShader(id, fragShader);
}

void OpenGLShader::processShaders(const std::string& vertSrc, 
								  const std::string& tescSrc, 
								  const std::string& teseSrc, 
								  const std::string& geomSrc, 
								  const std::string& fragSrc) {

	GLuint vertexShader			= compileShader(GL_VERTEX_SHADER, vertSrc);
	GLuint tessControlShader	= compileShader(GL_TESS_CONTROL_SHADER, tescSrc);
	GLuint tessEvaluationShader = compileShader(GL_TESS_EVALUATION_SHADER, teseSrc);
	GLuint geometryShader		= compileShader(GL_GEOMETRY_SHADER, geomSrc);
	GLuint fragmentShader		= compileShader(GL_FRAGMENT_SHADER, fragSrc);

	m_rendererID = glCreateProgram();

	linkShaders(m_rendererID, vertexShader, tessControlShader, tessEvaluationShader, geometryShader, fragmentShader);
}

OpenGLShader::OpenGLShader(const std::initializer_list<ShaderInfo>& files) {
	std::vector<ShaderInfo> infos(files);
	
	std::string srcs[NumShaderTypes]{ "" };
	for (auto& info : infos) {
		ASSERT(srcs[info.type - 1].empty(), "Multiple same type shaders defined!");
		std::ifstream i(info.filename);
		ASSERT(i, "File \"{0}\" could not open!", info.filename);
		std::stringstream ss;
		ss << i.rdbuf();
		srcs[info.type - 1] = ss.str();
	}

	processShaders(srcs[0], srcs[1], srcs[2], srcs[3], srcs[4]);
}

OpenGLShader::~OpenGLShader() {
	glDeleteProgram(m_rendererID);
}

void OpenGLShader::bind() const {
	glUseProgram(m_rendererID);
}

void OpenGLShader::unbind() const {
	glUseProgram(GL_NONE);
}

void OpenGLShader::setUniform(const std::string& name, const int v) {
	int location;
	location = glGetUniformLocation(m_rendererID, name.c_str());
	//ASSERT(location, "Location Not Found!");
	glUniform1i(location, v);
}

void OpenGLShader::setUniform(const std::string& name, const glm::vec2& v){
	int location;
	location = glGetUniformLocation(m_rendererID, name.c_str());
	//ASSERT(location, "Location Not Found!");
	glUniform2fv(location, 1, glm::value_ptr(v));
}

void OpenGLShader::setUniform(const std::string& name, const glm::vec3& v){
	int location;
	location = glGetUniformLocation(m_rendererID, name.c_str());
	//ASSERT(location, "Location Not Found!");
	glUniform3fv(location, 1, glm::value_ptr(v));
}

void OpenGLShader::setUniform(const std::string& name, const glm::vec4& v){
	int location;
	location = glGetUniformLocation(m_rendererID, name.c_str());
	//ASSERT(location, "Location Not Found!");
	glUniform4fv(location, 1, glm::value_ptr(v));
}

void OpenGLShader::setUniform(const std::string& name, const glm::mat3& m){
	int location;
	location = glGetUniformLocation(m_rendererID, name.c_str());
	//ASSERT(location, "Location Not Found!");
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

void OpenGLShader::setUniform(const std::string& name, const glm::mat4& m){
	int location;
	location = glGetUniformLocation(m_rendererID, name.c_str());
	//ASSERT(location, "Location Not Found!");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

}

