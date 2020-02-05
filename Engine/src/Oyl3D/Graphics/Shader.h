#pragma once

namespace oyl
{
    namespace internal
    {
        class GuiLayer;
    }

    struct ShaderInfo;
    
    class Shader
    {
    public:
        enum Type
        {
            Vertex = 0,
            TessControl,
            TessEvaluation,
            Geometry,
            Pixel,
            Fragment = Pixel,
            NumShaderTypes,
        };
        
    public:
        virtual ~Shader() = default;

        virtual bool load(const std::vector<ShaderInfo>& infos) = 0;
        virtual bool load(const std::initializer_list<ShaderInfo>& infos) = 0;

        virtual void unload() = 0;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        static Ref<Shader> create(const std::vector<ShaderInfo>& infos);
        static Ref<Shader> create(const std::initializer_list<ShaderInfo>& files);

        static const Ref<Shader>& cache(const std::initializer_list<ShaderInfo>& files, 
                                        const CacheAlias& alias, 
                                        bool overwrite = false);

        static const Ref<Shader>& cache(const std::vector<ShaderInfo>& infos,
                                        const CacheAlias& alias,
                                        bool overwrite = false);

        static const Ref<Shader>& cache(const Ref<Shader>& shader,
                                        const CacheAlias& alias,
                                        bool overwrite = false);
        
        static void discard(const CacheAlias& alias);

        static const Ref<Shader>& get(const CacheAlias& alias);

        static bool isCached(const Ref<Shader>& existing);

        static const CacheAlias& getAlias(const Ref<Shader>& existing);

        static bool exists(const CacheAlias& alias);
        
        static const Ref<Shader>& rename(const CacheAlias& currentAlias,
                                         const CacheAlias& newAlias,
                                         bool overwrite = false);

        static const auto& getCache() { return s_cache; }

        virtual void setUniform1i(const std::string& name, int v) = 0;
        virtual void setUniform1f(const std::string& name, float v) = 0;
        virtual void setUniform2f(const std::string& name, const glm::vec2& v) = 0;
        virtual void setUniform3f(const std::string& name, const glm::vec3& v) = 0;
        virtual void setUniform4f(const std::string& name, const glm::vec4& v) = 0;
        virtual void setUniformMat3(const std::string& name, const glm::mat3& v) = 0;
        virtual void setUniformMat4(const std::string& name, const glm::mat4& v) = 0;

        const std::vector<ShaderInfo>& getShaderInfos() { return m_shaderInfos; }

    protected:
        std::vector<ShaderInfo> m_shaderInfos;

    private:
        static Ref<Shader> s_invalid;

        static std::map<CacheAlias, Ref<Shader>> s_cache;
    };

    struct ShaderInfo
    {
        Shader::Type type;

        std::string filename;
    };
}
