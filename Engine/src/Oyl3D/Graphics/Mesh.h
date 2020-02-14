#pragma once

// TODO: Forward declare
//#include "Oyl3D/Graphics/Buffer.h"

#include "Oyl3D/Utils/AssetCache.h"

namespace oyl
{
    class VertexBuffer;
    class VertexArray;

    namespace internal
    {
        class AnimationSystem;    
    }
    
    class Mesh
    {
        friend class Application;
        friend internal::AnimationSystem;

        struct _Mesh {};

    public:
        struct FileInfo;

        explicit Mesh(_Mesh, const std::string& filePath);

        bool loadFromFile(const std::string& filePath);
        bool loadFromFile_(const std::string& filepath);
        void unload();

        void bind();
        void unbind();

        static Ref<Mesh> create(const std::string& filePath);

        // TODO: add cache for existing Ref
        static const Ref<Mesh>& cache(const std::string& filePath,
                                      const CacheAlias&  alias = "",
                                      bool               overwrite = false);

        static const Ref<Mesh>& cache(const Ref<Mesh>&  existing,
                                      const CacheAlias& alias = "",
                                      bool              overwrite = false);

        static void discard(const CacheAlias& alias);

        static const Ref<Mesh>& get(const CacheAlias& alias);

        static bool isCached(const Ref<Mesh>& existing);

        static bool exists(const CacheAlias& alias);
        
        static const CacheAlias& getAlias(const Ref<Mesh>& existing);

        static const Ref<Mesh>& rename(const CacheAlias& currentAlias,
                                       const CacheAlias& newAlias,
                                       bool overwrite = false);

        static const auto& getCache() { return s_cache.m_cache; }

        const Ref<VertexArray>& getVertexArray() const { return m_vao; }
        const Ref<VertexBuffer>& getVertexBuffer() const { return m_vbo; }

        uint getNumFaces() const { return m_numFaces; }
        uint getNumVertices() const { return m_numVertices; }
        
        const std::string& getFilePath() { return m_filePath; }

    protected:
        uint m_numFaces    = 0;
        uint m_numVertices = 0;

        Ref<VertexArray>  m_vao = nullptr;
        Ref<VertexBuffer> m_vbo = nullptr;

        std::string m_filePath;

        static internal::AssetCache<Mesh> s_cache;

    private:
        static void init();
    };
}
