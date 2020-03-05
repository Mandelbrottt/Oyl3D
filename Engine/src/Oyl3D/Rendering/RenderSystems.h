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

            void onEnter() override;

            void onUpdate() override;

            bool onEvent(const Event& event) override;
            
        private:
            glm::ivec2 m_windowSize = { 1, 1 };

            bool m_camerasNeedUpdate = false;

            //Ref<Shader> m_skyboxShader;
        };

        extern void skeletonAnimate(entt::entity entity, component::Renderable& renderable, component::SkeletonAnimatable& sa);
        
        class ForwardRenderSystem : public System
        {
            OYL_CTOR(ForwardRenderSystem, System)

            void onEnter() override;

            void onUpdate() override;
        };

        class DeferredRenderSystem : public System
        {
            OYL_CTOR(DeferredRenderSystem, System)

            void onEnter() override;

            void onUpdate() override;

        private:
            Ref<Shader> m_deferredPostShader;

            Ref<VertexArray> m_fullscreenQuad;
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

        class PostRenderSystem : public System
        {
            OYL_CTOR(PostRenderSystem, System)

            void onEnter() override;

            void onUpdate() override;

        private:
            Ref<Shader> m_skyboxShader;
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
