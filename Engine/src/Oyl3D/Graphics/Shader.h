#pragma once

namespace oyl
{
    struct ShaderInfo
    {
        OylEnum     type;
        std::string filename;
    };

    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        static Ref<Shader> create(const std::initializer_list<ShaderInfo>& files);

        virtual void setUniform1i(const std::string& name, const int v) = 0;
        virtual void setUniform1f(const std::string& name, const float v) = 0;
        virtual void setUniform2f(const std::string& name, const glm::vec2& v) = 0;
        virtual void setUniform3f(const std::string& name, const glm::vec3& v) = 0;
        virtual void setUniform4f(const std::string& name, const glm::vec4& v) = 0;
        virtual void setUniformMat3(const std::string& name, const glm::mat3& v) = 0;
        virtual void setUniformMat4(const std::string& name, const glm::mat4& v) = 0;
    };
}
