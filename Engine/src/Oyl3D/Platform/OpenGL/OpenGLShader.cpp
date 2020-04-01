#include "oylpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace oyl
{
    static GLenum shaderTypeFromOylEnum(Shader::Type e)
    {
        switch (e)
        {
            case Shader::Vertex: return GL_VERTEX_SHADER;
            case Shader::TessControl: return GL_TESS_CONTROL_SHADER;
            case Shader::TessEvaluation: return GL_TESS_EVALUATION_SHADER;
            case Shader::Geometry: return GL_GEOMETRY_SHADER;
            case Shader::Fragment: return GL_FRAGMENT_SHADER;
        }
        return GL_NONE;
    }

    uint OpenGLShader::compileShader(Shader::Type type, const std::string& src)
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
                case Vertex:
                    err.insert(0, "Vertex");
                    break;
                case TessControl:
                    err.insert(0, "Tesselation Control");
                    break;
                case TessEvaluation:
                    err.insert(0, "Tesselation Evaluation");
                    break;
                case Geometry:
                    err.insert(0, "Geometry");
                    break;
                case Fragment:
                    err.insert(0, "Fragment");
                    break;
            }
            OYL_LOG_ERROR(err.c_str());

            return -1u;
        }
        return shader;
    }

    u32 OpenGLShader::linkShaders(const std::array<uint, NumShaderTypes>& shaders)
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

    void OpenGLShader::processShaders(const std::array<std::string, NumShaderTypes>& shaderSrcs)
    {
        std::array<uint, NumShaderTypes> shaders{ 0 };

        bool failed = false;
        
        // Get the IDs for all of the present shaders
        for (uint i = 0; i < NumShaderTypes; i++)
        {
            shaders[i] = compileShader((Type) i, shaderSrcs[i]);
            failed |= shaders[i] == -1u;
        }

        m_rendererID = failed ? 0 : linkShaders(shaders);
    }

    void OpenGLShader::processShaders(const std::string& compoundSrc)
    {
        std::array<uint, NumShaderTypes> shaders{ 0 };

        bool failed = false;

        enum TypeMask : uint { NONE = 0x00, VERT = 0x01, TESC = 0x02, TESE = 0x04, GEOM = 0x08, FRAG = 0x10 };

        uint foundMask = NONE;
        foundMask |= compoundSrc.find("OYL_VERT") != std::string::npos ? VERT : 0;
        foundMask |= compoundSrc.find("OYL_TESC") != std::string::npos ? TESC : 0;
        foundMask |= compoundSrc.find("OYL_TESE") != std::string::npos ? TESE : 0;
        foundMask |= compoundSrc.find("OYL_GEOM") != std::string::npos ? GEOM : 0;
        foundMask |= compoundSrc.find("OYL_FRAG") != std::string::npos ? FRAG : 0;

        std::string define = "#define OYL_VERT 1\r\n";
        
        std::string modifiedSrc;
        modifiedSrc.reserve(compoundSrc.length() + define.length());
        size_t pos = compoundSrc.find("#version");
        pos = compoundSrc.find_first_of("\r\n", pos) + 1;
        modifiedSrc.assign(compoundSrc, 0, pos);
        modifiedSrc.append(define);
        modifiedSrc.append(compoundSrc, pos);
        
        // Get the IDs for all of the present shaders
        for (uint i = 0; i < NumShaderTypes; i++)
        {
            if (foundMask & (1u << i))
            {
                shaders[i] = compileShader((Type) i, modifiedSrc);
                failed |= shaders[i] == -1u;
            }

            switch (static_cast<Type>(i))
            {
                case Vertex:
                    modifiedSrc.replace(pos, define.length(), "#define OYL_TESC 1\r\n"); break;
                case TessControl:
                    modifiedSrc.replace(pos, define.length(), "#define OYL_TESE 1\r\n"); break;
                case TessEvaluation:
                    modifiedSrc.replace(pos, define.length(), "#define OYL_GEOM 1\r\n"); break;
                case Geometry:
                    modifiedSrc.replace(pos, define.length(), "#define OYL_FRAG 1\r\n"); break;
            }
        }

        m_rendererID = failed ? 0 : linkShaders(shaders);
    }

    OpenGLShader::OpenGLShader(_OpenGLShader, const std::string& filename)
    {
        OpenGLShader::load(filename);
    }
    
    OpenGLShader::OpenGLShader(_OpenGLShader, const std::vector<ShaderInfo>& infos)
    {
        OpenGLShader::load(infos);
    }

    OpenGLShader::~OpenGLShader()
    {
        OpenGLShader::unload();
    }

    bool OpenGLShader::load(const std::string& filename)
    {
        std::array<std::string, NumShaderTypes> srcs{ "" };
        
        std::ifstream in(filename);
        // Return if the file is not opened correctly
        if (!in)
        {
            OYL_LOG_ERROR("Shader \"{0}\" could not open!", filename.c_str());
            return false;
        }

        // Dump the entire file contents into the corresponding source string
        std::stringstream ss;
        ss << in.rdbuf();
        std::string src = ss.str();

        uint prevRendererID = m_rendererID;

        // Compile and link the given shader source codes into one program
        processShaders(src);

        if (!m_rendererID)
        {
            m_rendererID = prevRendererID;
            OYL_LOG_ERROR("Shader \"{}\" failed to compile!", filename);
            return false;
        }

        m_uniformLocations.clear();

        if (prevRendererID)
            glDeleteProgram(prevRendererID);

        m_shaderInfos = { { Type::Compound, filename } };
        return true;
    }

    bool OpenGLShader::load(const std::vector<ShaderInfo>& infos)
    {
        if (infos.size() == 1 && infos[0].type == Shader::Type::Compound)
            return OpenGLShader::load(infos[0].filename);
        
        std::array<std::string, NumShaderTypes> srcs{ "" };
        for (auto& info : infos)
        {
            // Return if the source code for a given shader already exists
            if (!srcs[info.type].empty())
            {
                OYL_LOG_ERROR("Multiple same type shaders defined! Duplicate File: {0}", info.filename.c_str());
                continue;
            }
            std::ifstream in(info.filename);
            // Return if the file is not opened correctly
            if (!in)
            {
                OYL_LOG_ERROR("Shader \"{0}\" could not open!", info.filename.c_str());
                return false;
            }

            // Dump the entire file contents into the corresponding source string
            std::stringstream ss;
            ss << in.rdbuf();
            srcs[info.type] = ss.str();
        }

        uint prevRendererID = m_rendererID;

        // Compile and link the given shader source codes into one program
        processShaders(srcs);

        if (!m_rendererID)
        {
            m_rendererID = prevRendererID;
            return false;
        }

        m_uniformLocations.clear();

        if (prevRendererID) 
            glDeleteProgram(prevRendererID);
        
        m_shaderInfos = infos;
        return true;
    }
    
    void OpenGLShader::unload()
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
            OYL_LOG_ERROR("Invalid uniform name \"{0}\"!", name.c_str());
        }

        return location;
    }

    void OpenGLShader::setUniform1i(const std::string& name, int v)
    {
        if (GLint location = getUniformLocation(name); location != -1)
            glUniform1i(location, v);
    }

    void OpenGLShader::setUniform1f(const std::string& name, float v)
    {
        if (GLint location = getUniformLocation(name); location != -1)
            glUniform1f(location, v);
    }

    void OpenGLShader::setUniform2f(const std::string& name, const glm::vec2& v)
    {
        if (GLint location = getUniformLocation(name); location != -1)
            glUniform2fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniform3f(const std::string& name, const glm::vec3& v)
    {
        if (GLint location = getUniformLocation(name); location != -1)
            glUniform3fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniform4f(const std::string& name, const glm::vec4& v)
    {
        if (GLint location = getUniformLocation(name); location != -1)
            glUniform4fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniformMat3(const std::string& name, const glm::mat3& m)
    {
        if (GLint location = getUniformLocation(name); location != -1)
            glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m));
    }

    void OpenGLShader::setUniformMat4(const std::string& name, const glm::mat4& m)
    {
        if (GLint location = getUniformLocation(name); location != -1)
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
    }

    void OpenGLShader::setUniform1i(int location, int v)
    {
        glUniform1i(location, v);
    }
    
    void OpenGLShader::setUniform1f(int location, float v)
    {
        glUniform1f(location, v);
    }
    
    void OpenGLShader::setUniform2f(int location, const glm::vec2& v)
    {
        glUniform2fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniform3f(int location, const glm::vec3& v)
    {
        glUniform3fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniform4f(int location, const glm::vec4& v)
    {
        glUniform4fv(location, 1, glm::value_ptr(v));
    }

    void OpenGLShader::setUniformMat3(int location, const glm::mat3& m)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m));
    }

    void OpenGLShader::setUniformMat4(int location, const glm::mat4& m)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
    }
}
