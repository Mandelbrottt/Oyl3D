#include "oylpch.h"
#include "AnimationSystems.h"

#include "Components/Animatable.h"

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
        auto view = registry->view<component::Animatable>();
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
}
