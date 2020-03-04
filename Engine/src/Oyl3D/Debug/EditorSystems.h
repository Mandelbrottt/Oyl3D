#pragma once
#include "Scenes/System.h"

namespace oyl
{
    class FrameBuffer;
    class EditorCamera;
    class Shader;

    namespace internal
    {
        class EditorCameraSystem : public System
        {
            OYL_CTOR(EditorCameraSystem, System)

            virtual void onEnter() override;
            virtual void onExit() override;

            virtual void onUpdate() override;
            virtual void onGuiRender() override;

            virtual bool onEvent(const Event& event) override;

        private:
            Ref<EditorCamera> m_camera;

            glm::vec3 m_cameraMove        = glm::vec3(0.0f);
            float     m_cameraMoveSpeed   = 15.0f;
            glm::vec3 m_cameraRotate      = glm::vec3(0.0f);
            float     m_cameraRotateSpeed = 25.0f;

            bool m_doMoveCamera = false;
        };

        class EditorRenderSystem : public System
        {
            OYL_CTOR(EditorRenderSystem, System)

            virtual void onEnter() override;
            virtual void onExit() override;

            virtual void onUpdate() override;
            virtual void onGuiRender() override;

            virtual bool onEvent(const Event& event) override;

            void init();
            void shutdown();

        private:
            Ref<FrameBuffer> m_editorViewportBuffer;

            Ref<EditorCamera> m_targetCamera;

            glm::ivec2 m_windowSize;
            Ref<Shader> m_skyboxShader;
        };
    }
}
