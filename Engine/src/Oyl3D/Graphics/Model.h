#pragma once

#include "Mesh.h"

#include "Oyl3D/Utils/AssetCache.h"

#include "Oyl3D/Animation/SkeletonAnimation.h"

struct aiAnimation;
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

        void getBoneTransforms(const std::string& name, float time, std::vector<glm::mat4>& out) const;

        const std::unordered_map<std::string, SkeletonAnimation>& getAnimations() const { return m_animations; }
        
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
        Mesh processMesh(aiMesh* a_mesh, const aiScene* a_scene);
        void processNode(aiNode* a_node);
        void processAnimation(aiAnimation* a_animation);

        void calculateFinalTransform(const SkeletonAnimation& animation,
                                     float                    time,
                                     uint                     bone,
                                     const glm::mat4&         parentTransform) const;

        glm::vec3 calcInterpolatedPosition(float time, const BoneChannel& channel) const;
        glm::quat calcInterpolatedRotation(float time, const BoneChannel& channel) const;
        glm::vec3 calcInterpolatedScale(float time, const BoneChannel& channel) const;
        
        std::vector<Mesh> m_meshes;

        std::string m_filepath;

        glm::mat4 m_globalInverseTransform = glm::mat4(1.0f);

        std::unordered_map<std::string, uint> m_boneIDs;

        std::vector<Bone> m_bones;

        std::unordered_map<std::string, SkeletonAnimation> m_animations;

        float m_unitScale = 1.0f;

        static internal::AssetCache<Model> s_cache;
    };
}