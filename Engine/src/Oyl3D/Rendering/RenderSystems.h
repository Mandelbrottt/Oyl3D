#pragma once

#include "Scenes/System.h"

namespace oyl
{
    class Shader;
    class VertexArray;
    class FrameBuffer;
    
    namespace internal
    {
        class RenderSystem : public System
        {
            OYL_CTOR(RenderSystem, System)

            virtual void onEnter() override;
            virtual void onExit() override;

            virtual void onUpdate() override;
            virtual void onGuiRender() override;

            virtual bool onEvent(const Event& event) override;

        private:
            glm::ivec2 m_windowSize;

            bool m_camerasNeedUpdate = false;
        };

        class GuiRenderSystem : public System
        {
            OYL_CTOR(GuiRenderSystem, System)

            virtual void onEnter() override;
            virtual void onExit() override;

            virtual void onUpdate() override;
            virtual void onGuiRender() override;

            virtual bool onEvent(const Event& event) override;

        private:
            glm::ivec2 m_windowSize;

            Ref<Shader> m_shader;

            Ref<VertexArray> m_vao;
        };

        class PostRenderSystem : public System
        {
            OYL_CTOR(PostRenderSystem, System)

            virtual void onEnter() override;
            virtual void onExit() override;

            virtual void onUpdate() override;
            virtual void onGuiRender() override;

            virtual bool onEvent(const Event& event) override;

        private:
            glm::ivec2 m_windowSize;

            Ref<FrameBuffer> m_forwardFrameBuffer;
            Ref<FrameBuffer> m_intermediateFrameBuffer;
            Ref<VertexArray> m_vao;
            Ref<Shader>      m_shader;
        };
    }
}
