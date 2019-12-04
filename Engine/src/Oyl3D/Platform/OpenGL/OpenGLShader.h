#pragma once

#include "Graphics/Shader.h"

namespace oyl
{
    // TODO: Store every individual shader (ie. vert or frag) in a global list
    //       On create, index into list with provided filenames and see if they match
    //       link existing shaders into new program
    class OpenGLShader : public Shader
    {
        struct _OpenGLShader {};

        friend class Shader;
    public:
        explicit OpenGLShader(_OpenGLShader, const std::vector<ShaderInfo>& infos);
        virtual  ~OpenGLShader();

        virtual bool load(const std::vector<ShaderInfo>& infos) override;
        virtual bool load(const std::initializer_list<ShaderInfo>& infos) override;

        virtual void unload() override;

        virtual void bind() const override;
        virtual void unbind() const override;

        virtual void setUniform1i(const std::string& name, int v) override;
        virtual void setUniform1f(const std::string& name, float v) override;
        virtual void setUniform2f(const std::string& name, const glm::vec2& v) override;
        virtual void setUniform3f(const std::string& name, const glm::vec3& v) override;
        virtual void setUniform4f(const std::string& name, const glm::vec4& v) override;
        virtual void setUniformMat3(const std::string& name, const glm::mat3& m) override;
        virtual void setUniformMat4(const std::string& name, const glm::mat4& m) override;

        static Ref<Shader> create(const std::vector<ShaderInfo>& infos)
        {
            return Ref<OpenGLShader>::create(_OpenGLShader{}, infos);
        }

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
