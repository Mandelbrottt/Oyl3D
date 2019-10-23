#pragma once

#include "Oyl3D/Graphics/Buffer.h"

namespace oyl
{
    class Mesh
    {
        friend class Application;

        struct _Mesh
        {
        };

    public:
        using CacheAlias = std::string;

        explicit Mesh(_Mesh, const std::string& filePath);

        bool loadFromFile(const std::string& filePath);
        void unload();

        void bind();
        void unbind();

        static Ref<Mesh> create(const std::string& filePath);

        static const Ref<Mesh>& cache(const std::string& filePath,
                                      CacheAlias         alias     = "",
                                      bool               overwrite = false);

        static void discard(const CacheAlias& alias);

        static const Ref<Mesh>& get(const CacheAlias& alias);

        static const Ref<Mesh>& rename(const CacheAlias& currentAlias,
                                       const CacheAlias& newAlias,
                                       bool              overwrite = false);

        uint getNumFaces() const { return m_numFaces; }
        uint getNumVertices() const { return m_numVertices; }

    protected:
        uint m_numFaces    = 0;
        uint m_numVertices = 0;

        Ref<VertexArray>  m_vao = nullptr;
        Ref<VertexBuffer> m_vbo = nullptr;

        std::string m_fileName;

        static Ref<Mesh> s_invalid;

        static std::unordered_map<CacheAlias, Ref<Mesh>> s_cache;

    private:
        static void init();
    };
}
