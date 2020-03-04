#pragma once

#include "Oyl3D/oylpch.h"

#include "Oyl3D/Utils/Interpolation.h"

namespace oyl
{
    class Scene;
    class Mesh;
    class VertexArray;
    
    namespace internal
    {
        class AnimationSystem;
        class ForwardRenderSystem;
        class SkeletalAnimationSystem;
    }
    
    struct Animation
    {
        struct KeyPose
        {
            Ref<Mesh> mesh;
            f32       duration = 0.0f;
        };

        std::vector<KeyPose> poses;
    };
}

namespace oyl
{
    namespace component
    {
        class VertexAnimatable
        {
        public:
            void pushAnimation(const std::string& alias, const Ref<Animation>& animation);

            void setNextAnimation(const std::string& alias, f32 transitionDuration = 0.0f);
            
            bool getBool(const std::string& name) const;
            void setBool(const std::string& name, bool value);

            const Ref<VertexArray>& getVertexArray() const;
        private:
            friend internal::AnimationSystem;
            friend internal::ForwardRenderSystem;
            friend internal::DeferredRenderSystem;
            std::unordered_map<std::string, Ref<Animation>> m_animations;

            std::unordered_map<std::string, bool> m_bools;

            Ref<Animation> m_currentAnimation;
            Ref<Animation> m_nextAnimation;

            Ref<VertexArray> m_vao;

            f32 m_currentElapsed = 0.0f;

            f32 m_transitionElapsed  = 0.0f;
            f32 m_transitionDuration = 0.0f;
        };

        struct SkeletonAnimatable
        {
            bool play    = true;
            bool reverse = false;
            bool loop    = true;

            float time = 0.0f;
            float timeScale = 1.0f;

            Interpolation::Type lerpType = Interpolation::Type::Cubic;
            Interpolation::EaseFn easeFn = Interpolation::linear;
            
            std::string animation;
            
        private:
            std::string prevAnimation;

            friend ::oyl::internal::SkeletalAnimationSystem;
        };

        struct BoneTarget
        {
            entt::entity target;

            std::string bone;

            glm::mat4 forceUpdateTransform();

        private:
            friend ::oyl::Scene;

            static void on_construct(entt::entity entity, entt::registry& registry, BoneTarget& boneTarget);

            entt::entity m_owner = entt::null;
            
            entt::registry* m_registry;
        };
    }
}

#include "Animatable.inl"
