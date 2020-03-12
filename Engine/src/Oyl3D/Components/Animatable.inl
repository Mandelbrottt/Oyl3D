#pragma once

#include "Animatable.h"
#include "Renderable.h"
#include "Transform.h"

#include "Oyl3D/Animation/SkeletonAnimation.h"

namespace oyl
{
    namespace component
    {
        inline void VertexAnimatable::pushAnimation(const std::string& alias, const Ref<Animation>& animation)
        {
            OYL_ASSERT(m_animations.find(alias) == m_animations.end(), "Animation '{0}' already exists!", alias);

            m_animations[alias] = animation;
        }

        inline void VertexAnimatable::setNextAnimation(const std::string& alias, f32 transitionDuration)
        {
            auto it = m_animations.find(alias);
            OYL_ASSERT(it != m_animations.end(), "Animation '{0}' was not found!", alias);

            m_nextAnimation      = it->second;
            m_transitionElapsed  = 0.0f;
            m_transitionDuration = transitionDuration;
        }

        inline bool VertexAnimatable::getBool(const std::string& name) const
        {
            OYL_ASSERT(m_bools.find(name) != m_bools.end(), "Bool '{0}' was not found!", name);

            return m_bools.at(name);
        }

        inline void VertexAnimatable::setBool(const std::string& name, bool value)
        {
            m_bools[name] = value;
        }

        inline const Ref<VertexArray>& VertexAnimatable::getVertexArray() const
        {
            return m_vao;
        }

        inline glm::mat4 BoneTarget::forceUpdateTransform()
        {
            using component::Transform;
            using component::Renderable;
            using component::SkeletonAnimatable;

            if (!m_registry->valid(target))
                return glm::mat4(1.0f);

            auto* pTransform  = m_registry->try_get<Transform>(m_owner);
            auto* pRenderable = m_registry->try_get<Renderable>(target);
            auto* pAnimatable = m_registry->try_get<SkeletonAnimatable>(target);

            if (!(pTransform && pRenderable && pRenderable->model && pAnimatable))
                return glm::mat4(1.0f);

            auto& model = pRenderable->model;
            glm::mat4 boneTransform = model->getBoneTransform(pAnimatable->animation, bone, pAnimatable->time);

            glm::vec3 scale;
            glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 translation = glm::vec3(0.0f);
            glm::vec3 _skew;
            glm::vec4 _perspective;
            
            if (decompose(boneTransform, scale, orientation, translation, _skew, _perspective))
            {
                pTransform->setPosition(translation);

                orientation = normalize(orientation);

                pTransform->setRotation(orientation);

                return boneTransform;
            }

            return pTransform->getMatrix();
        }

        inline void BoneTarget::on_construct(entt::entity entity, entt::registry& registry, BoneTarget& boneTarget)
        {
            boneTarget.m_owner    = entity;
            boneTarget.m_registry = &registry;
        }
    }
}
