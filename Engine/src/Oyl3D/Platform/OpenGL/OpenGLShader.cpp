#include "oylpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace oyl
{
    static GLenum shaderTypeFromOylEnum(const uint e)
    {
        switch (e)
        {
            case VertexShader: return GL_VERTEX_SHADER;
            case TessControlShader: return GL_TESS_CONTROL_SHADER;
            case TessEvaluationShader: return GL_TESS_EVALUATION_SHADER;
            case GeometryShader: return GL_GEOMETRY_SHADER;
            case FragmentShader: return GL_FRAGMENT_SHADER;
        }
        return GL_NONE;
    }

    uint OpenGLShader::compileShader(uint type, const std::string& src)
    {
        if (src.empty()) return 0;

        GLuint shader = glCreateShader(shaderTypeFromOylEnum(type));

        // Upload the shader source code to the GPU
        const char* source = src.c_str();
        glShaderSource(shader, 1, &source, 0);

        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

        if (isCompiled == GL_FALSE)
        {
            // Retreive the error log from OpenGL
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(shader);

            // Print the error log
            OYL_LOG_ERROR("{0}", infoLog.data());
            std::string err = " shader compilation failure!";
            switch (type)
            {
                case VertexShader:
                    err.insert(0, "Vertex");
                    break;
                case TessControlShader:
                    err.insert(0, "Tesselation Control");
                    break;
                case TessEvaluationShader:
                    err.insert(0, "Tesselation Evaluation");
                    break;
                case GeometryShader:
                    err.insert(0, "Geometry");
                    break;
                case FragmentShader:
                    err.insert(0, "Fragment");
                    break;
            }
            OYL_LOG_ERROR(err.c_str());

            return 0;
        }
        return shader;
    }

    uint OpenGLShader::linkShaders(std::array<uint, NumShaderTypes> shaders)
    {
        // Get a program ID from OpenGL
        GLuint program = glCreateProgram();
        if (program == 0)
        {
            OYL_ASSERT(false, "Shader program failed to be created!");
            return 0;
        }

        // Attach all of the present shaders to the shader program
        for (auto shader : shaders)
        {
            if (shader != 0)
                glAttachShader(program, shader);
        }

        glLinkProgram(program);

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            // Retreive the error log from OpenGL
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(program);

            // Always delete shaders after a failed link
            for (auto shaderID : shaders)
            {
                if (shaderID != 0)
                    glDeleteShader(shaderID);
            }

            OYL_ASSERT(false, "Shader link failure!\n{0}", infoLog.data());

            return 0;
        }

        // Always detach shaders after a successful link
        for (auto shaderID : shaders)
        {
            if (shaderID != 0)
                glDetachShader(program, shaderID);
        }

        return program;
    }

    void OpenGLShader::processShaders(std::array<std::string, NumShaderTypes> shaderSrcs)
    {
        std::array<uint, NumShaderTypes> shaders{ 0 };

        // Get the IDs for all of the present shaders
        for (uint i = 0; i < NumShaderTypes; i++)
        {
            shaders[i] = compileShader(VertexShader + i, shaderSrcs[i]);
        }

        m_rendererID = linkShaders(shaders);
    }

    OpenGLShader::OpenGLShader(const std::initializer_list<ShaderInfo>& files)
    {
        std::vector<ShaderInfo> infos(files);

        std::array<std::string, NumShaderTypes> srcs{ "" };
        for (auto& info : infos)
        {
            // Return if the source code for a given shader already exists
            if (!srcs[info.type - 1].empty())
            {
                OYL_LOG_ERROR("Multiple same type shaders defined! Duplicate File: {0}", info.filename.c_str());
                continue;
            }
            std::ifstream in(info.filename);
            // Return if the file is not opened correctly
            if (!in)
            {
                OYL_LOG_ERROR("Shader \"{0}\" could not open!", info.filename.c_str());
                return;
            }

            // Dump the entire file contents into the corresponding source string
            std::stringstream ss;
            ss << in.rdbuf();
            srcs[info.type - 1] = ss.str();
        }

        // Compile and link the given shader source codes into one program
        processShaders(srcs);
    }

    OpenGLShader::~OpenGLShader()
    {
        if (m_rendererID)
            glDeleteProgram(m_rendererID);
    }

    void OpenGLShader::bind() const
    {
        glUseProgram(m_rendererID);
    }

    void OpenGLShader::unbind() const
    {
        glUseProgram(GL_NONE);
    }

    GLint OpenGLShader::getUniformLocation(const std::string& name) const
    {
        // Return the cached uniform location
        if (m_uniformLocations.find(name) != m_uniformLocations.end())
            return m_uniformLocations.at(name);

        GLint location = glGetUniformLocation(m_rendererID, name.c_str());

        if (location != -1)
        {
            // Cache the uniform location
            m_uniformLocations[name] = location;
        }
        else
        {
            OYL_ASSERT(false, "Invalid uniform name \"{0}\"!", name.c_str());
        }

        return location;
    }

    void OpenGLShader::setUniform1i(const std::string& name, const int v)
    {
        GLint location = getUniformLocation(name);
        glUniform1i(location, v);
    }

    void OpenGLShader::setUniform1f(const std::string& name, const float v)
    {
        GLint location = getUniformLocation(name);
        glUniform1f(location, v);
    }

    void OpenGLShader::setUniform2f(const std::string& name, const glm::vec2& v)
    {
        GLint location = getUniformLocation(name);
        glUniform2fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniform3f(const std::string& name, const glm::vec3& v)
    {
        GLint location = getUniformLocation(name);
        glUniform3fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniform4f(const std::string& name, const glm::vec4& v)
    {
        GLint location = getUniformLocation(name);
        glUniform4fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniformMat3(const std::string& name, const glm::mat3& m)
    {
        GLint location = getUniformLocation(name);
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m));
    }

    void OpenGLShader::setUniformMat4(const std::string& name, const glm::mat4& m)
    {
        GLint location = getUniformLocation(name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
    }
}
