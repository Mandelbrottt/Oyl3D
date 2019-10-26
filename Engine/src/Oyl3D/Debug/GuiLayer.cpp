#include "oylpch.h"
#include "GuiLayer.h"

#include "App/Application.h"

#include "ECS/Component.h"
#include "ECS/Registry.h"

#include "Graphics/Camera.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

static const char* entityNodeFmt = "%s\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

namespace oyl
{
    void GuiLayer::onEnter()
    {
        setupGuiLibrary();

        addToEventMask(TypeViewportHandleChanged);
        addToEventMask(TypeEditorEntitySelected);
        addToEventMask(TypeMousePressed);

        scheduleSystemUpdate<ECS::internal::OracleCameraSystem>();

        ImGuizmo::SetGizmoScale(2.0f);
        ImGuizmo::SetGizmoThickness(2.0f);
    }

    void GuiLayer::setupGuiLibrary()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGuiStyle& style                 = ImGui::GetStyle();
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        Application& app    = Application::get();
        GLFWwindow*  window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 420");
    }

    void GuiLayer::onExit()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GuiLayer::begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = (ImGuiWindowFlags_NoDocking |
                                         ImGuiWindowFlags_NoTitleBar |
                                         ImGuiWindowFlags_NoCollapse |
                                         ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoBringToFrontOnFocus |
                                         ImGuiWindowFlags_NoNavFocus |
                                         ImGuiWindowFlags_MenuBar);

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        bool p_open = true;
        ImGui::Begin("DockSpace##EditorDockspace", &p_open, window_flags);
        ImGui::PopStyleVar(3);

        drawMenuBar();

        OYL_ASSERT(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable, "Docking should always be enabled!");
        
        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID("_DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        ImGui::End();
    }

    void GuiLayer::end()
    {
        ImGuiIO&     io  = ImGui::GetIO();
        Application& app = Application::get();
        io.DisplaySize   = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void GuiLayer::onGuiRender(Timestep dt)
    {
        drawSceneHierarchy();

        drawInspector();

        drawViewport();
    }

    bool GuiLayer::onEvent(Ref<Event> event)
    {
        switch (event->type)
        {
            case TypeViewportHandleChanged:
            {
                auto e           = (ViewportHandleChangedEvent) *event;
                m_viewportHandle = e.handle;
                return true;
            }
            case TypeEditorEntitySelected:
            {
                auto e          = (EditorEntitySelectedEvent) *event;
                m_currentEntity = u32(e.entity);
                return true;
            }
            case TypeMousePressed:
            {
                //auto e = (MousePressedEvent) *event;
                //if (e.button == Mouse_Right)
                //{
                //    m_currentEntity = -1;
                //}
            }
        }
        return false;
    }

    void GuiLayer::drawMenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File##MainMenuBarFile"))
            {
                ImGui::Text("Nothing to see here ;)");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit##MainMenuBarFile"))
            {
                ImGui::Text("Nothing to see here ;)");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Windows##MainMenuBarWindows"))
            {
                ImGui::Text("Nothing to see here ;)");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help##MainMenuBarHelp"))
            {
                ImGui::Text("Nothing to see here ;)");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void GuiLayer::drawSceneHierarchy()
    {
        if (ImGui::Begin("Hierarchy##EditorHierarchy", nullptr, ImGuiWindowFlags_NoCollapse))
        {
            registry->each(
                [this](auto entity)
                {
                    auto& so = registry->get_or_assign<component::SceneObject>(entity);
                    if (so.name.empty())
                    {
                        char buf[128];
                        sprintf(buf, "Entity %d", (u32) entity);
                        so.name = std::string(buf);
                    }

                    auto nodeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                     ImGuiTreeNodeFlags_OpenOnArrow |
                                     ImGuiTreeNodeFlags_DefaultOpen;

                    if (entity == Entity(m_currentEntity))
                        nodeFlags |= ImGuiTreeNodeFlags_Selected;

                    bool treeNode = ImGui::TreeNodeEx((const void*) entity, nodeFlags, entityNodeFmt, so.name.c_str());
                    bool clicked  = ImGui::IsItemClicked(0);
                    if (treeNode)
                    {
                        ImGui::Indent();

                        ImGui::Text("Nothing Here Yet!");

                        ImGui::Unindent();

                        ImGui::TreePop();
                    }
                
                    float testValue = ImGui::GetMousePos().x - ImGui::GetItemRectMin().x;
                    if (clicked && testValue > ImGui::GetTreeNodeToLabelSpacing())
                    {
                        EditorEntitySelectedEvent selected;
                        selected.entity = entity;
                        postEvent(Event::create(selected));
                    }
                });
        }
        ImGui::End();
    }

    void GuiLayer::drawInspector()
    {
        if (ImGui::Begin("Inspector##EditorInspector", nullptr))
        {
            if (m_currentEntity != u32(-1))
            {
                drawInspectorTransform();
                drawInspectorRenderable();
                drawInspectorRigidBody();
            }
        }
        ImGui::End();
    }

    void GuiLayer::drawInspectorTransform()
    {
        if (ImGui::CollapsingHeader("Transform##InspectorTransform"), 0, ImGuiTreeNodeFlags_DefaultOpen)
        {
            ImGui::Indent(5);

            auto& transform = registry->get_or_assign<component::Transform>(Entity(m_currentEntity));

            float newWidth = ImGui::GetWindowContentRegionWidth() / 6;

            ImGui::PushItemWidth(newWidth);

            const float posDragSpeed = 0.02f;
            ImGui::Text("Position");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 20));
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##XPos", &transform.position.x, posDragSpeed, 0, 0, "X");
            ImGui::SameLine();
            ImGui::InputFloat("##XPosInput", &transform.position.x, 0, 0, "%.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##YPos", &transform.position.y, posDragSpeed, 0, 0, "Y");
            ImGui::SameLine();
            ImGui::InputFloat("##YPosInput", &transform.position.y, 0, 0, "%.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##ZPos", &transform.position.z, posDragSpeed, 0, 0, "Z");
            ImGui::SameLine();
            ImGui::InputFloat("##ZPosInput", &transform.position.z, 0, 0, "%.2f");

            const float rotDragSpeed = 0.5f;
            ImGui::Text("Rotation");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 20));
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##XRot", &transform.rotation.x, rotDragSpeed, 0, 0, "X");
            ImGui::SameLine();
            ImGui::InputFloat("##XRotInput", &transform.rotation.x, 0, 0, "%.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##YRot", &transform.rotation.y, rotDragSpeed, 0, 0, "Y");
            ImGui::SameLine();
            ImGui::InputFloat("##YRotInput", &transform.rotation.y, 0, 0, "%.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##ZRot", &transform.rotation.z, rotDragSpeed, 0, 0, "Z");
            ImGui::SameLine();
            ImGui::InputFloat("##ZRotInput", &transform.rotation.z, 0, 0, "%.2f");

            const float scaleDragSpeed = 0.02f;
            ImGui::Text("Scale");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 20));
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##XSca", &transform.scale.x, scaleDragSpeed, 0, 0, "X");
            ImGui::SameLine();
            ImGui::InputFloat("##XScaInput", &transform.scale.x, 0, 0, "%.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##YSca", &transform.scale.y, scaleDragSpeed, 0, 0, "Y");
            ImGui::SameLine();
            ImGui::InputFloat("##YScaInput", &transform.scale.y, 0, 0, "%.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##ZSca", &transform.scale.z, scaleDragSpeed, 0, 0, "Z");
            ImGui::SameLine();
            ImGui::InputFloat("##ZScaInput", &transform.scale.z, 0, 0, "%.2f");

            ImGui::IsItemClicked();

            ImGui::PopItemWidth();

            ImGui::Unindent(5);
        }
    }

    void GuiLayer::drawInspectorRenderable()
    {
    }

    void GuiLayer::drawInspectorRigidBody()
    {
    }

    void GuiLayer::drawViewport()
    {
        // Only still here for easy navigation to the source code for learning imgui
        if constexpr (false)
            ImGui::ShowDemoWindow();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport##EditorViewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

        auto [x, y]       = ImGui::GetWindowSize();
        auto [cx, cy]     = ImGui::GetCursorPos();
        auto [posx, posy] = ImGui::GetItemRectMin();

        y -= cy;

        float camX = posx + x - 40;
        float camY = posy + y;

        ImGui::SetNextWindowPos(ImVec2(camX, camY), 0, ImVec2(1, 1));
        ImVec2 cameraWindowSize = ImVec2(x / 4, y / 4);
        ImGui::SetNextWindowSize(cameraWindowSize);

        if (ImGui::Begin("Camera Preview##ViewportCameraPreview", nullptr,
                         ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove))
        {
            ImGui::Image(
                (void*) m_viewportHandle,
                cameraWindowSize,
                ImVec2(0, 1), ImVec2(1, 0)
            );
        }

        ImGui::PopStyleVar();
        ImGui::End();

        ViewportResizedEvent vrevent;
        vrevent.id     = 0;
        vrevent.width  = x;
        vrevent.height = y;

        postEvent(Event::create(vrevent));

        ImGui::Image(
            (void*) m_viewportHandle,
            ImVec2(x, y),
            ImVec2(0, 1), ImVec2(1, 0)
        );

        ImGuizmo::SetDrawlist();
        auto [minX, minY]   = ImGui::GetItemRectMin();
        auto [sizeX, sizeY] = ImGui::GetItemRectSize();
        ImGuizmo::SetRect(minX, minY, sizeX, sizeY);

        if (ImGui::IsItemClicked(1))
            m_currentEntity = -1;

        drawTransformGizmo();

        //ImVec2 gzsPos = ImVec2(minX + 20, minY + cy + 20);
        //ImGui::SetNextWindowPos(gzsPos);

        drawTransformGizmoSettings();

        ImGui::End();
    }

    void GuiLayer::drawTransformGizmoSettings()
    {
        if (ImGui::Begin("##ViewportGizmoSettings", NULL, ImGuiWindowFlags_NoDecoration))
        {
            if (ImGui::RadioButton("Translate##RadioTranslate", m_currentOp == ImGuizmo::TRANSLATE))
                m_currentOp = ImGuizmo::TRANSLATE;
            ImGui::SameLine(0, 5);
            if (ImGui::RadioButton("Rotate##RadioRotate", m_currentOp == ImGuizmo::ROTATE))
                m_currentOp = ImGuizmo::ROTATE;
            ImGui::SameLine(0, 5);
            if (ImGui::RadioButton("Scale##RadioScale", m_currentOp == ImGuizmo::SCALE))
                m_currentOp = ImGuizmo::SCALE;
            ImGui::SameLine(0, 30);

            if (ImGui::RadioButton("World##RadioWorld", m_currentMode == ImGuizmo::WORLD))
                m_currentMode = ImGuizmo::WORLD;
            ImGui::SameLine(0, 5);
            if (ImGui::RadioButton("Local##RadioLocal", m_currentMode == ImGuizmo::LOCAL))
                m_currentMode = ImGuizmo::LOCAL;
            ImGui::SameLine(0, 30);

            ImGui::Checkbox("##CheckboxSnap", &m_doSnap);

            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);

            switch (m_currentOp)
            {
                case ImGuizmo::TRANSLATE:
                    ImGui::InputFloat("Snap##InputSnapTranslate", &m_translateSnap.x, 0.1f, 0.5f, "%.2f");
                    m_translateSnap.z = m_translateSnap.y = m_translateSnap.x;
                    m_snap = m_translateSnap;
                    break;
                case ImGuizmo::ROTATE:
                    ImGui::InputFloat("Snap##InputSnapRotate", &m_rotateSnap.x, 1.0f, 5.0f, "%.2f");
                    m_rotateSnap.z = m_rotateSnap.y = m_rotateSnap.x;
                    m_snap = m_rotateSnap;
                    break;
                case ImGuizmo::SCALE:
                    ImGui::InputFloat("Snap##InputSnapScale", &m_scaleSnap.x, 0.1f, 0.5f, "%.2f");
                    m_scaleSnap.z = m_scaleSnap.y = m_scaleSnap.x;
                    m_snap = m_scaleSnap;
                    break;
            }
        }
        ImGui::End();
    }

    void GuiLayer::drawTransformGizmo()
    {
        using component::Transform;
        using component::internal::EditorCamera;

        if (m_currentEntity != u32(-1) &&
            registry->has<Transform>(Entity(m_currentEntity)))
        {
            auto  view = registry->view<EditorCamera>();
            auto& cam  = view.get(*view.begin()).camera;

            auto& model = registry->get<Transform>(Entity(m_currentEntity));

            glm::mat4 modelMatrix;

            ImGuizmo::RecomposeMatrixFromComponents(value_ptr(model.position),
                                                    value_ptr(model.rotation),
                                                    value_ptr(model.scale),
                                                    value_ptr(modelMatrix));

            ImGuizmo::Manipulate(value_ptr(cam->getViewMatrix()),
                                 value_ptr(cam->getProjectionMatrix()),
                                 m_currentOp,
                                 m_currentMode,
                                 value_ptr(modelMatrix),
                                 nullptr,
                                 m_doSnap ? &m_snap.x : nullptr);

            ImGuizmo::DecomposeMatrixToComponents(value_ptr(modelMatrix),
                                                  value_ptr(model.position),
                                                  value_ptr(model.rotation),
                                                  value_ptr(model.scale));

            model.scale = glm::max(glm::vec3(0.01f), model.scale);
        }
    }
}
