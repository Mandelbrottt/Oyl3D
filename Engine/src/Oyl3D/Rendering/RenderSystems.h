#pragma once

#include "Scenes/System.h"
#include "Graphics/Material.h"
#include "Utils/Logging.h"

namespace oyl
{
    class Shader;
    class VertexArray;
    class FrameBuffer;

    namespace component
    {
        class Renderable;
        class SkeletonAnimatable;
    }
    
    namespace internal
    {
        class PreRenderSystem : public System
        {
            OYL_CTOR(PreRenderSystem, System)

            void onUpdate() override;
        };
        
        class CameraRenderSetupSystem : public System
        {
            OYL_CTOR(CameraRenderSetupSystem, System)

            void onEnter() override;
            void onExit() override;

            void onUpdate() override;
            void onGuiRender() override;

            bool onEvent(const Event& event) override;

        private:
            glm::ivec2 m_windowSize;

            bool m_camerasNeedUpdate = false;
        };

        extern void skeletonAnimate(entt::entity entity, component::Renderable& renderable, component::SkeletonAnimatable& sa);
        
        class ForwardRenderSystem : public System
        {
            OYL_CTOR(ForwardRenderSystem, System)

            void onEnter() override;
            void onExit() override;

            void onUpdate() override;
            void onGuiRender() override;

            bool onEvent(const Event& event) override;

        private:
            glm::ivec2 m_windowSize;

            bool m_camerasNeedUpdate = false;

        };

        class DeferredRenderSystem : public System
        {
            OYL_CTOR(DeferredRenderSystem, System)

            void onEnter() override;
            void onExit() override;

            void onUpdate() override;
            void onGuiRender() override;

            bool onEvent(const Event& event) override;

        private:
            glm::ivec2 m_windowSize;

            bool m_camerasNeedUpdate = false;

            void skeletonAnimate(entt::entity entity, component::Renderable& renderable, component::SkeletonAnimatable& sa);
        };

        class GuiRenderSystem : public System
        {
            OYL_CTOR(GuiRenderSystem, System)

            void onEnter() override;
            void onExit() override;

            void onUpdate() override;
            void onGuiRender() override;

            bool onEvent(const Event& event) override;

        private:
            glm::ivec2 m_windowSize;

            Ref<Shader> m_shader;

            Ref<VertexArray> m_vao;
        };

        class ShadowRenderSystem : public System
        {
            OYL_CTOR(ShadowRenderSystem, System)

            void onEnter() override;
            void onExit() override;

            void onUpdate() override;
            void onGuiRender() override;

            bool onEvent(const Event& event) override;

        private:
            Ref<Shader> m_shader;
        };

        class UserPostRenderSystem : public System
        {
            OYL_CTOR(UserPostRenderSystem, System)

            void onEnter() override;
            void onExit() override;

            void onUpdate() override;
            void onGuiRender() override;

            bool onEvent(const Event& event) override;

        private:
            glm::ivec2 m_windowSize;

            Ref<FrameBuffer> m_forwardFrameBuffer;
            Ref<FrameBuffer> m_intermediateFrameBuffer;
            Ref<VertexArray> m_vao;
            Ref<Shader>      m_shader;
        };
    }
}
