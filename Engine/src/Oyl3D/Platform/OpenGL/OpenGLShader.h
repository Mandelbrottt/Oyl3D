#pragma once

#include "Graphics/Shader.h"

namespace oyl
{
    class OpenGLShader : public Shader
    {
        struct _OpenGLShader
        {
        };

        friend class Shader;
    public:
        explicit OpenGLShader(_OpenGLShader, const std::initializer_list<ShaderInfo>& files);
        virtual  ~OpenGLShader();

        virtual void bind() const override;
        virtual void unbind() const override;

        virtual void setUniform1i(const std::string& name, int v) override;
        virtual void setUniform1f(const std::string& name, float v) override;
        virtual void setUniform2f(const std::string& name, const glm::vec2& v) override;
        virtual void setUniform3f(const std::string& name, const glm::vec3& v) override;
        virtual void setUniform4f(const std::string& name, const glm::vec4& v) override;
        virtual void setUniformMat3(const std::string& name, const glm::mat3& m) override;
        virtual void setUniformMat4(const std::string& name, const glm::mat4& m) override;

        using _InitList = std::initializer_list<ShaderInfo>;
        static Ref<Shader> create(const _InitList& files) { return Ref<OpenGLShader>::create(_OpenGLShader{}, files); }

    private:
        void processShaders(std::array<std::string, NumShaderTypes> shaderSrcs);
        uint compileShader(uint type, const std::string& src);
        uint linkShaders(std::array<uint, NumShaderTypes> shaders);

        int getUniformLocation(const std::string& name) const;
    private:
        uint m_rendererID = 0;

        mutable std::unordered_map<std::string, int> m_uniformLocations;
    };
}
