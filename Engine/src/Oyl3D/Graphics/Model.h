#pragma once

#include "Mesh.h"

#include "Oyl3D/Utils/AssetCache.h"

struct aiMesh;
struct aiNode;
struct aiScene;

namespace oyl
{    
    class Model
    {
        struct _Model {};

    public:
        explicit Model(_Model, const std::string& filepath);

        void loadFromFile(const std::string& filepath);

        //void bind();
        //void unbind();

        const std::vector<Mesh>& getMeshes() const { return m_meshes; }

        const std::string& getFilePath() const { return m_filepath; }

        static void init();

        static Ref<Model> create(const std::string& filepath);

        static const Ref<Model>& cache(const std::string& filepath,
                                       const CacheAlias& alias = "",
                                       bool              overwrite = false);
        
        static const Ref<Model>& cache(const Ref<Model>&  existing,
                                       const CacheAlias& alias = "",
                                       bool              overwrite = false);

        static void discard(const CacheAlias& alias);

        static const Ref<Model>& get(const CacheAlias& alias);

        static bool isCached(const Ref<Model>& existing);

        static bool exists(const CacheAlias& alias);
        
        static const CacheAlias& getAlias(const Ref<Model>& existing);

        static const Ref<Model>& rename(const CacheAlias& currentAlias,
                                        const CacheAlias& newAlias,
                                        bool overwrite = false);

        static const auto& getCache() { return s_cache.m_cache; }

    private:
        void processNode(aiNode* a_node, const aiScene* a_scene);
        Mesh processMesh(aiMesh* a_mesh, const aiScene* a_scene);
        
        std::vector<Mesh> m_meshes;

        std::string m_filepath;

        static internal::AssetCache<Model> s_cache;

    };
}