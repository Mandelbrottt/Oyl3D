#include "oylpch.h"
#include "AnimationSystems.h"

#include "Components/Animatable.h"
#include "Components/Renderable.h"

#include "Events/Event.h"

#include "Graphics/Buffer.h"
#include "Graphics/EditorCamera.h"
#include "Graphics/Mesh.h"

#include "Rendering/Renderer.h"

namespace oyl::internal
{
    void AnimationSystem::onEnter() {}

    void AnimationSystem::onExit() {}

    void AnimationSystem::onUpdate()
    {
        auto view = registry->view<component::VertexAnimatable>();
        for (auto entity : view)
        {
            auto& anim = view.get(entity);

            if (!anim.m_currentAnimation)
                anim.m_currentAnimation = anim.m_animations.begin()->second;

            if (!anim.m_vao)
            {
                anim.m_vao = VertexArray::create();

                anim.m_vao->addVertexBuffer(anim.m_currentAnimation->poses[0].mesh->getVertexBuffer());
                anim.m_vao->addVertexBuffer(anim.m_currentAnimation->poses[1].mesh->getVertexBuffer());
            }

            if (anim.m_nextAnimation)
            {
                anim.m_transitionElapsed += Time::deltaTime() / anim.m_transitionDuration;
                if (anim.m_transitionElapsed >= 1.0f)
                {
                    anim.m_currentAnimation = anim.m_nextAnimation;
                    anim.m_nextAnimation.reset();
                    anim.m_transitionElapsed  = 0.0f;
                    anim.m_transitionDuration = 0.0f;
                    anim.m_currentElapsed     = -0.0001f;
                }
            }

            anim.m_currentElapsed =
                glm::mod(anim.m_currentElapsed, (f32) anim.m_currentAnimation->poses.size());

            uint lastVal = glm::floor(anim.m_currentElapsed);
            anim.m_currentElapsed += Time::deltaTime() / anim.m_currentAnimation->poses[lastVal].duration;

            anim.m_currentElapsed =
                glm::mod(anim.m_currentElapsed, (f32) anim.m_currentAnimation->poses.size());

            uint currVal = glm::floor(anim.m_currentElapsed);

            if (lastVal != currVal)
            {
                ++lastVal %= anim.m_currentAnimation->poses.size();
                ++currVal %= anim.m_currentAnimation->poses.size();

                auto lastMeshVbo = anim.m_currentAnimation->poses[lastVal].mesh->getVertexBuffer();
                auto currMeshVbo = anim.m_currentAnimation->poses[currVal].mesh->getVertexBuffer();

                anim.m_vao->unload();
                anim.m_vao->load();

                anim.m_vao->addVertexBuffer(lastMeshVbo);
                anim.m_vao->addVertexBuffer(currMeshVbo);
                if (anim.m_nextAnimation)
                {
                    auto transMeshVbo = anim.m_nextAnimation->poses[0].mesh->getVertexBuffer();
                    anim.m_vao->addVertexBuffer(transMeshVbo);
                }
            }
        }
    }

    void AnimationSystem::onGuiRender() {}

    bool AnimationSystem::onEvent(const Event& event) { return false; }
    
    void SkeletalAnimationSystem::onEnter() {}

    void SkeletalAnimationSystem::onExit() {}

    void SkeletalAnimationSystem::onUpdate()
    {
        using component::SkeletonAnimatable;
        using component::Renderable;
        auto view = registry->view<SkeletonAnimatable, Renderable>();
        view.each([](auto entity, SkeletonAnimatable& sa, Renderable& renderable)
        {
            if (renderable.model && renderable.material && renderable.material->shader == Shader::get("Oyl Skeletal"))
            {
                if (auto it = renderable.model->getAnimations().find(sa.animation); 
                    it != renderable.model->getAnimations().end())
                {
                    auto& animation = it->second;

                    std::vector<glm::mat4> boneTransforms;
                    renderable.model->getBoneTransforms(sa.animation, sa.time, boneTransforms);

                    renderable.material->shader->bind();
                    for (uint i = 0; i < 64; i++)
                    {
                        glm::mat4 uniform = glm::mat4(1.0f);
                        if (i < boneTransforms.size())
                            uniform = boneTransforms[i];
                        
                        renderable.material->shader->setUniformMat4("u_boneTransforms[" + std::to_string(i) + "]", uniform);
                    }

                    if (sa.play)
                    {
                        float dt = Time::deltaTime();
                        if (sa.reverse) dt *= -1.0f;

                        sa.time += dt * sa.timeScale;

                        float t_mod_d = abs(fmod(sa.time, animation.duration));
                        
                        if (sa.time < 0.0f)
                        {
                            sa.time = animation.duration - t_mod_d;

                            if (!sa.loop) 
                                sa.play = false;
                        }
                        else
                        {
                            if (sa.time > animation.duration && !sa.loop) 
                                sa.play = false;
                            
                            sa.time = t_mod_d;
                        }
                    }
                }
                else
                {
                    renderable.material->shader->bind();
                    for (uint i = 0; i < 64; i++)
                        renderable.material->shader->setUniformMat4("u_boneTransforms[" + std::to_string(i) + "]", glm::mat4(1.0f));
                }
            }
        });

        registry->view<component::BoneTarget>().each([&](component::BoneTarget& boneTarget)
        {
            boneTarget.forceUpdateTransform();
        });
    }

    void SkeletalAnimationSystem::onGuiRender() {}

    bool SkeletalAnimationSystem::onEvent(const Event& event)
    {
        return false;
    }
}
