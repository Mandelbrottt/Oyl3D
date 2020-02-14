#pragma once

#include "Scenes/System.h"
#include "Graphics/Material.h"
#include "Utils/Logging.h"

namespace oyl
{
    class Shader;
    class VertexArray;
    class FrameBuffer;
    
    namespace internal
    {
        class PreRenderSystem : public System
        {
            OYL_CTOR(PreRenderSystem, System)

            virtual void onUpdate() override;
        };
        
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

        class ShadowRenderSystem : public System
        {
            OYL_CTOR(ShadowRenderSystem, System)

            virtual void onEnter() override;
            virtual void onExit() override;

            virtual void onUpdate() override;
            virtual void onGuiRender() override;

            virtual bool onEvent(const Event& event) override;

        private:
            Ref<Shader> m_shader;
        };

        class UserPostRenderSystem : public System
        {
            OYL_CTOR(UserPostRenderSystem, System)

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
