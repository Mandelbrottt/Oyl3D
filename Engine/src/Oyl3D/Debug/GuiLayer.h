#pragma once

#include "Oyl3D/Scenes/Layer.h"
#include "Selectable.h"

// TODO: Turn off mouse input when cursor is disabled

namespace oyl
{
    class EditorCamera;

    namespace component
    {
        class EntityInfo;
        class Transform;
    }
    
    namespace internal
    {
        class GuiLayer : public Layer
        {
            friend class EditorCommand;
        public:
            OYL_CTOR(GuiLayer, Layer)

            void onEnter() override;
            void onExit() override;
            void onUpdate() override;
            void onGuiRender() override;

            bool onEvent(const Event& event) override;

            void init();
            void shutdown();

            void begin();
            void end();

            bool doGameUpdate() const { return !m_editorOverrideUpdate && m_gameUpdate; }

        private:
            void setupGuiLibrary();
            void setupLayout();

            //void addToCommandHistory(UniqueRef<EditorCommand>&& command);

            void drawMenuBar();

            void drawSceneHierarchy();
            void drawEntityNode(entt::entity entity, component::EntityInfo& info, component::Transform& transform);
            void recursiveCopy(entt::entity copy, entt::entity original);
            void recursiveDelete(entt::entity entity);

            void setEntityParent(entt::entity entity, entt::entity parent);

            template<class T>
            bool userRemoveComponent();

            void drawInspector();
            void drawInspectorObjectName();
            void drawInspectorTransform();
            void drawInspectorCollidable();
            void drawInspectorRenderable();
            void drawInspectorGuiRenderable();
            void drawInspectorRigidBody();
            void drawInspectorPointLight();
            void drawInspectorDirectionalLight();
            void drawInspectorSpotLight();
            void drawInspectorCamera();
            void drawInspectorAddComponent();

            void drawInspectorMaterial();

            void drawAssetList();
            void drawAssetNode(const std::filesystem::directory_iterator& dir);

            void updateAssetList();
            bool updateAsset(const std::string& filepath,
                             void (*            loadAsset)(void*),
                             void*              userData);

            void drawAssetCache();
            void drawAnimationCache();
            void drawMaterialCache();
            void drawModelCache();
            void drawTextureCache();

            void drawSceneViewport();
            void drawGameViewport();

            void drawTransformGizmoSettings();
            void drawTransformGizmo();
            void drawPlayPauseButtons();

            void applyCustomColorTheme();

            ImGuiID m_consoleDockSpaceId;

            u32 m_editorViewportHandle;
            u32 m_gameViewportHandle;

            Selectable m_currentSelection;

            ImGuizmo::OPERATION m_currentOp   = ImGuizmo::TRANSLATE;
            ImGuizmo::MODE      m_currentMode = ImGuizmo::WORLD;

            glm::vec3 m_translateSnap{ 0.5f };
            glm::vec3 m_rotateSnap{ 15.0f };
            glm::vec3 m_scaleSnap{ 0.2f };
            glm::vec3 m_snap;

            entt::registry m_registryRestore;
            entt::registry m_prefabRegistry;

            bool m_doSnap               = false;
            bool m_editorOverrideUpdate = true;
            bool m_gameUpdate           = false;

            u32 m_currentCommandPos;

            std::unordered_map<std::string, std::fs::file_time_type> m_fileSaveTimes;
            decltype(m_fileSaveTimes)::iterator                      m_fileSaveTimeIt;

            Ref<EditorCamera> m_editorCamera;
        };

        template<class T>
        bool GuiLayer::userRemoveComponent()
        {
            if (ImGui::BeginPopupContextItem())
            {
                bool removed = false;
                if (ImGui::Selectable("Delete Component"))
                    registry->remove<T>(m_currentSelection.entity()), removed = true;
                return ImGui::EndPopup(), removed;
            }
            return false;
        }
    }
}
