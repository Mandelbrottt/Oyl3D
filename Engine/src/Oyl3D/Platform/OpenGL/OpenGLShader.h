#pragma once

#include "Graphics/Shader.h"
#include <complex.h>

namespace oyl
{
    // TODO: Store every individual shader (ie. vert or frag) in a global list
    //       On create, index into list with provided filenames and see if they match
    //       link existing shaders into new program
    class OpenGLShader final : public Shader
    {
        struct _OpenGLShader {};

        friend class Shader;
    public:
        explicit OpenGLShader(_OpenGLShader, const std::vector<ShaderInfo>& infos);
        virtual  ~OpenGLShader();

        bool load(const std::vector<ShaderInfo>& infos) override;
        bool load(const std::initializer_list<ShaderInfo>& infos) override;

        void unload() override;

        void bind() const override;
        void unbind() const override;

        void setUniform1i(const std::string& name, int v) override;
        void setUniform1f(const std::string& name, float v) override;
        void setUniform2f(const std::string& name, const glm::vec2& v) override;
        void setUniform3f(const std::string& name, const glm::vec3& v) override;
        void setUniform4f(const std::string& name, const glm::vec4& v) override;
        void setUniformMat3(const std::string& name, const glm::mat3& m) override;
        void setUniformMat4(const std::string& name, const glm::mat4& m) override;

        void setUniform1i(int location, int v) override;
        void setUniform1f(int location, float v) override;
        void setUniform2f(int location, const glm::vec2& v) override;
        void setUniform3f(int location, const glm::vec3& v) override;
        void setUniform4f(int location, const glm::vec4& v) override;
        void setUniformMat3(int location, const glm::mat3& m) override;
        void setUniformMat4(int location, const glm::mat4& m) override;

        static Ref<Shader> create(const std::vector<ShaderInfo>& infos)
        {
            return Ref<OpenGLShader>::create(_OpenGLShader{}, infos);
        }

    private:
        void processShaders(const std::array<std::string, NumShaderTypes>& shaderSrcs);
        uint compileShader(Shader::Type type, const std::string& src);
        uint linkShaders(const std::array<u32, NumShaderTypes>& shaders);

        int getUniformLocation(const std::string& name) const;
    private:
        uint m_rendererID = 0;

        mutable std::unordered_map<std::string, int> m_uniformLocations;
    };
}
