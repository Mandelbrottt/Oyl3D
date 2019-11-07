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
        void onUpdate(Timestep dt) override;
        void onGuiRender(Timestep dt) override;

        bool onEvent(Ref<Event> event) override;

        void init();
        void shutdown();

        void begin();
        void end();

        bool doGameUpdate() const { return !m_editorOverrideUpdate && m_gameUpdate; }

    private:
        void setupGuiLibrary();
        void setupLayout();
        
        void drawMenuBar();

        void drawSceneHierarchy();
        void drawEntityNode(entt::entity entity);
        
        void drawInspector();
        void drawInspectorTransform();
        void drawInspectorRenderable();
        void drawInspectorRigidBody();
        void drawInspectorParent();
        void drawInspectorObjectName();
        
        void drawSceneViewport();
        void drawGameViewport();

        void drawTransformGizmoSettings();
        void drawTransformGizmo();
        void drawPlayPauseButtons();

        void applyCustomColorTheme();

    private:
        ImGuiID m_consoleDockSpaceId;

        u32 m_editorViewportHandle;
        u32 m_gameViewportHandle;

        entt::entity m_currentEntity = entt::null;

        ImGuizmo::OPERATION m_currentOp = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE m_currentMode = ImGuizmo::WORLD;

        glm::vec3 m_translateSnap{ 0.5f };
        glm::vec3 m_rotateSnap{ 15.0f };
        glm::vec3 m_scaleSnap{ 0.2f };
        glm::vec3 m_snap;

        entt::registry m_registryRestore;
        
        bool m_doSnap = false;
        bool m_editorOverrideUpdate = true;
        bool m_gameUpdate = false;
    };
}
