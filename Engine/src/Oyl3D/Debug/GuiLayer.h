#pragma once

#include "Oyl3D/Scenes/Layer.h"

// TODO: Turn off mouse input when cursor is disabled

namespace oyl
{
    class GuiLayer : public Layer
    {
    public:
        OYL_CTOR(GuiLayer, Layer)

        void onEnter() override;
        void onExit() override;
        void onGuiRender(Timestep dt) override;

        bool onEvent(Ref<Event> event) override;

        void begin();
        void end();

    private:
        void setupGuiLibrary();
        void drawMenuBar();
        void drawSceneHierarchy();
        void drawInspector();
        void drawInspectorTransform();
        void drawInspectorRenderable();
        void drawInspectorRigidBody();
        void drawViewport();
        void drawTransformGizmoSettings();
        void drawTransformGizmo();

        void applyCustomColorTheme();

        u32 m_viewportHandle;

        Entity m_currentEntity = Entity(-1);

        ImGuizmo::OPERATION m_currentOp = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE m_currentMode = ImGuizmo::WORLD;

        bool m_doSnap = false;
        glm::vec3 m_translateSnap{ 0.5f };
        glm::vec3 m_rotateSnap{ 15.0f };
        glm::vec3 m_scaleSnap{ 0.2f };
        glm::vec3 m_snap;
    };
}
