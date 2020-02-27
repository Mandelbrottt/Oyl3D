#pragma once

namespace oyl
{
    class VertexBuffer;
    class VertexArray;
    
    struct VertexData
    {
        glm::vec3  position{};
        glm::vec2  textureCoords{};
        glm::vec3  normal{};
        glm::vec3  tangent{};
        glm::vec3  biTangent{};
        glm::ivec4 bones{};
        glm::vec4  boneWeights{};
    };
    
    class Mesh
    {
    public:
        //explicit Mesh(_Mesh, VertexData* a_vertices, uint a_numVertices);
        Mesh() = default;
        explicit Mesh(VertexData* a_vertices, uint a_numVertices);

        void loadFromVertices(VertexData* a_vertices, uint a_numVertices);
        void unload();

        void bind();
        void unbind();

        const Ref<VertexArray>& getVertexArray() const { return m_vao; }
        const Ref<VertexBuffer>& getVertexBuffer() const { return m_vbo; }

        uint getNumFaces() const { return m_numFaces; }
        uint getNumVertices() const { return m_numVertices; }
        
        //static Ref<Mesh> create(VertexData* a_vertices, uint a_numVertices);

        //static const Ref<Mesh>& cache(VertexData* a_vertices, uint a_numVertices,
        //                              const CacheAlias& alias = "",
        //                              bool              overwrite = false);
        //
        //static const Ref<Mesh>& cache(const Ref<Mesh>&  existing,
        //                              const CacheAlias& alias = "",
        //                              bool              overwrite = false);

        //static void discard(const CacheAlias& alias);

        //static const Ref<Mesh>& get(const CacheAlias& alias);

        //static bool isCached(const Ref<Mesh>& existing);

        //static bool exists(const CacheAlias& alias);
        //
        //static const CacheAlias& getAlias(const Ref<Mesh>& existing);

        //static const Ref<Mesh>& rename(const CacheAlias& currentAlias,
        //                               const CacheAlias& newAlias,
        //                               bool overwrite = false);

        //static const auto& getCache() { return s_cache.m_cache; }

    protected:
        uint m_numFaces    = 0;
        uint m_numVertices = 0;

        Ref<VertexArray>  m_vao = nullptr;
        Ref<VertexBuffer> m_vbo = nullptr;

        //static internal::AssetCache<Mesh> s_cache;
    };

    // TODO: Introduce Mesh Builder API
}
