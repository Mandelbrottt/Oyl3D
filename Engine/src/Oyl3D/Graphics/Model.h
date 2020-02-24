#pragma once

#include "Mesh.h"

#include "Oyl3D/Utils/AssetCache.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace oyl
{    
    class Model
    {
        struct _Model {};

    public:
        explicit Model(_Model, const std::string& filepath);
        ~Model();

        void loadFromFile(const std::string& filepath);

        //void bind();
        //void unbind();

        const std::vector<Mesh>& getMeshes() const { return m_meshes; }

        const std::string& getFilePath() const { return m_filepath; }

        void getBoneTransforms(const std::string& animation, float animTime, std::vector<glm::mat4>& transforms);
        void readNodeHierarchy(const std::string& animation, float a_animTime, const aiNode* a_node, glm::mat4 a_parentTransform);

        const std::unordered_map<std::string, const aiAnimation*>& getAnimations() const { return m_animations; }

        glm::vec3 calcInterpolatedPosition(float a_animTime, const aiNodeAnim* a_nodeAnim);
        glm::quat calcInterpolatedRotation(float a_animTime, const aiNodeAnim* a_nodeAnim);
        glm::vec3 calcInterpolatedScale(float a_animTime, const aiNodeAnim* a_nodeAnim);

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
        //void processNode(aiNode* a_node, const aiScene* a_scene);
        Mesh processMesh(aiMesh* a_mesh, const aiScene* a_scene);
        
        std::vector<Mesh> m_meshes;

        std::string m_filepath;

        static internal::AssetCache<Model> s_cache;

        glm::mat4 m_globalInverseTransform = glm::mat4(1.0f);

        std::unordered_map<std::string, uint> m_boneIndices;
        uint m_numBones = 0;

        struct BoneInfo
        {
            glm::mat4 transform;
            glm::mat4 inverseTransform;
            glm::mat4 finalTransform;
        };

        std::vector<BoneInfo> m_boneInfos;

        std::unordered_map<std::string, const aiAnimation*> m_animations;

        // Assimp specific
        Assimp::Importer* m_importer = nullptr;

        const aiScene* m_scene = nullptr;
    };
}