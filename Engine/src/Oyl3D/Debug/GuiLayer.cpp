#include "oylpch.h"
#include "GuiLayer.h"

#include "App/Application.h"

#include "ECS/Component.h"
#include "ECS/Registry.h"
#include "ECS/SystemImpl.h"

#include "Graphics/Camera.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <imgui_internal.h>

#include <GLFW/glfw3.h>

static const char* entityNodeFmt = "%s\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

namespace oyl
{
    void GuiLayer::init()
    {
        setupGuiLibrary();

        addToEventMask(TypeEditorViewportHandleChanged);
        addToEventMask(TypeEditorEntitySelected);

        addToEventMask(TypeViewportHandleChanged);

        addToEventMask(TypeMousePressed);

        ImGuizmo::SetGizmoScale(2.0f);
        ImGuizmo::SetGizmoThickness(2.0f);
    }
    
    void GuiLayer::shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    
    void GuiLayer::onEnter()
    {
        scheduleSystemUpdate<internal::EditorCameraSystem>();
        scheduleSystemUpdate<internal::EditorRenderSystem>();

        for (auto& system : m_systems)
        {
            system->setRegistry(registry);
            system->setDispatcher(m_dispatcher);
        }
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
        applyCustomColorTheme();
        
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
        for (auto& system : m_systems)
        {
            system->onExit();
        }
        
        m_systems.clear();
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

        drawSceneViewport();

        if (!m_editorOverrideUpdate)
            drawGameViewport();

        if (ImGui::Begin("##TestPlayButton"), NULL, ImGuiWindowFlags_NoDecoration)
        {
            if (m_editorOverrideUpdate)
            {
                if (ImGui::ArrowButton("##EditorPlayButton", ImGuiDir_Right))
                {
                    m_editorOverrideUpdate = false;
                    m_gameUpdate = true;
                    m_registryRestore = Scene::current()->getRegistry()->clone();
                    m_currentEntity = Entity(-1);
                }
            }
            else
            {
                if (ImGui::ArrowButton("##EditorBackButton", ImGuiDir_Left))
                {
                    m_editorOverrideUpdate = true;
                    m_gameUpdate = false;
                    *Scene::current()->m_registry = m_registryRestore.clone();
                    Scene::current()->m_physicsSystem->onExit();
                    Scene::current()->m_physicsSystem->onEnter();
                    m_currentEntity = Entity(-1);
                    
                }
                ImGui::SameLine();
                if ((m_gameUpdate && ImGui::Button("II##EditorPauseButton")) ||
                    (!m_gameUpdate && ImGui::ArrowButton("##EditorPausedPlayButton", ImGuiDir_Right)))
                {
                    m_gameUpdate ^= 1;
                }
            }
        }
        ImGui::End();
    }

    bool GuiLayer::onEvent(Ref<Event> event)
    {
        switch (event->type)
        {
            case TypeEditorViewportHandleChanged:
            {
                auto e = (EditorViewportHandleChangedEvent) *event;
                m_editorViewportHandle = e.handle;
                return false;
            }
            case TypeEditorEntitySelected:
            {
                auto e = (EditorEntitySelectedEvent) *event;
                m_currentEntity = e.entity;
                return true;
            }
            case TypeViewportHandleChanged:
            {
                auto e = (ViewportHandleChangedEvent) *event;
                m_gameViewportHandle = e.handle;
                return false;
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
        return !m_editorOverrideUpdate;
    }

    void GuiLayer::drawMenuBar()
    {
        static bool showReloadDialogue = false;
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File##MainMenuBarFile"))
            {
                if (ImGui::MenuItem("Save##MainMenuSave", "Ctrl+S", false, m_editorOverrideUpdate))
                {
                    Scene::current()->saveSceneToFile();   
                }
                showReloadDialogue = 
                    ImGui::MenuItem("Reload##MainMenuReload", "Ctrl+Shift+S", false, m_editorOverrideUpdate);
                
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

            if (showReloadDialogue)
            {
                ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
                if (ImGui::Begin("Confirmation##ReloadConfirmation", 0, 
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoCollapse))
                {
                    ImGui::Text("Are you sure you want to reload the scene?");
                    ImGui::Text("You will lose any unsaved changes you've made.");

                    ImGui::Indent(ImGui::GetWindowContentRegionWidth() / 2 - 55);
                    if (ImGui::Button("Reload##ReloadConfirmationReload"))
                    {
                        Scene::current()->loadSceneFromFile();
                        m_currentEntity = Entity(-1);
                        showReloadDialogue = false;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel##ReloadConfirmationCancel"))
                    {
                        showReloadDialogue = false;
                    }
                }
                ImGui::End();
            }
        }
    }

    void GuiLayer::drawSceneHierarchy()
    {
        if (ImGui::Begin("Hierarchy##EditorHierarchy", nullptr, ImGuiWindowFlags_NoCollapse))
        {
            registry->each(
                [this](auto entity)
                {
                    using component::internal::ExcludeFromHierarchy;
                    if (registry->has<ExcludeFromHierarchy>(entity))
                        return;
                
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
                    float testValue = ImGui::GetMousePos().x - ImGui::GetItemRectMin().x;
                    if (treeNode)
                    {
                        ImGui::Indent();

                        ImGui::Text("Nothing Here Yet!");

                        ImGui::Unindent();

                        ImGui::TreePop();
                    }
                
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
            if (m_currentEntity != Entity(-1))
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
        using component::Transform;
        
        if (registry->has<Transform>(m_currentEntity) &&
            ImGui::CollapsingHeader("Transform##InspectorTransform"))
        {
            ImGui::Indent(10);

            auto& transform = registry->get<Transform>(m_currentEntity);

            float newWidth = ImGui::GetWindowContentRegionWidth() / 6;

            ImGui::PushItemWidth(newWidth);

            {
                glm::vec3 position = transform.getPosition();

                const float posDragSpeed = 0.02f;
                ImGui::Text("Position");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##XPos", &position.x, posDragSpeed, 0, 0, "X");
                ImGui::SameLine();
                ImGui::InputFloat("##XPosInput", &position.x, 0, 0, "%.2f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##YPos", &position.y, posDragSpeed, 0, 0, "Y");
                ImGui::SameLine();
                ImGui::InputFloat("##YPosInput", &position.y, 0, 0, "%.2f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##ZPos", &position.z, posDragSpeed, 0, 0, "Z");
                ImGui::SameLine();
                ImGui::InputFloat("##ZPosInput", &position.z, 0, 0, "%.2f");

                if (position != transform.getPosition())
                    transform.setPosition(position);
            }
            {
                glm::vec3 rotation = transform.getRotationEuler();

                const float rotDragSpeed = 0.5f;
                ImGui::Text("Rotation");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##XRot", &rotation.x, rotDragSpeed, 0, 0, "X");
                ImGui::SameLine();
                ImGui::InputFloat("##XRotInput", &rotation.x, 0, 0, "%.2f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##YRot", &rotation.y, rotDragSpeed, 0, 0, "Y");
                ImGui::SameLine();
                ImGui::InputFloat("##YRotInput", &rotation.y, 0, 0, "%.2f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##ZRot", &rotation.z, rotDragSpeed, 0, 0, "Z");
                ImGui::SameLine();
                ImGui::InputFloat("##ZRotInput", &rotation.z, 0, 0, "%.2f");

                if (rotation != transform.getRotationEuler())
                    transform.setRotationEuler(rotation);
            }
            {
                glm::vec3 scale = transform.getScale();

                const float scaleDragSpeed = 0.02f;
                ImGui::Text("Scale");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##XSca", &scale.x, scaleDragSpeed, 0, 0, "X");
                ImGui::SameLine();
                ImGui::InputFloat("##XScaInput", &scale.x, 0, 0, "%.2f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##YSca", &scale.y, scaleDragSpeed, 0, 0, "Y");
                ImGui::SameLine();
                ImGui::InputFloat("##YScaInput", &scale.y, 0, 0, "%.2f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##ZSca", &scale.z, scaleDragSpeed, 0, 0, "Z");
                ImGui::SameLine();
                ImGui::InputFloat("##ZScaInput", &scale.z, 0, 0, "%.2f");

                if (scale != transform.getScale())
                    transform.setScale(scale);
            }

            ImGui::PopItemWidth();

            ImGui::Unindent(10);
        }
    }

    void GuiLayer::drawInspectorRenderable() {}

    void GuiLayer::drawInspectorRigidBody() {}

    void GuiLayer::drawSceneViewport()
    {
        // Only still here for easy navigation to the source code for learning imgui
        if constexpr (false)
            ImGui::ShowDemoWindow();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Scene##EditorSceneViewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
        {
            static ImVec2 lastSize = { 0, 0 };

            auto [x, y]   = ImGui::GetWindowSize();
            auto [cx, cy] = ImGui::GetCursorPos();

            if (lastSize.x != x || lastSize.y != y)
            {   
                EditorViewportResizedEvent vrevent;
                vrevent.width  = x;
                vrevent.height = y - cy;
                
                postEvent(Event::create(vrevent));
            }
            
            y -= cy;
    
            ImGui::Image(
                (void*) m_editorViewportHandle,
                ImVec2(x, y),
                ImVec2(0, 1), ImVec2(1, 0)
            );
    
            ImGuizmo::SetDrawlist();
            auto [minX, minY]   = ImGui::GetItemRectMin();
            auto [sizeX, sizeY] = ImGui::GetItemRectSize();
            ImGuizmo::SetRect(minX, minY, sizeX, sizeY);
    
            if (ImGui::IsItemClicked(1))
                m_currentEntity = Entity(-1);
    
            drawTransformGizmo();
        }

        drawTransformGizmoSettings();

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GuiLayer::drawGameViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Game##EditorGameViewport", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
        {
            // TEMPORARY:
            {
                auto [x, y] = ImGui::GetWindowSize();
                auto [cx, cy] = ImGui::GetCursorPos();

                //y -= cy;

                ImVec2 newPos = { 0, 0 };
                
                if (x / y < 16.0f / 9.0f)
                {
                    newPos.y = (y - x * 9.0f / 16.0f) / 2.0f;
                    y = x * 9.0f / 16.0f;
                }
                else
                {
                    newPos.x = (x - y * 16.0f / 9.0f) / 2.0f;
                    x = y * 16.0f / 9.0f;
                }

                ImGui::SetCursorPos(newPos);
                
                ImGui::Image(
                    (void*) m_gameViewportHandle,
                    ImVec2(x, y),
                    ImVec2(0, 1), ImVec2(1, 0)
                );
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
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
            ImGui::SetNextItemWidth(100);

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

        if (m_currentEntity != Entity(-1) &&
            registry->has<Transform>(Entity(m_currentEntity)))
        {
            auto  view = registry->view<EditorCamera>();
            auto& cam  = view.get(*view.begin()).camera;

            auto& model = registry->get<Transform>(Entity(m_currentEntity));

            glm::vec3 position = model.getPosition();
            glm::vec3 rotation = model.getRotationEuler();
            glm::vec3 scale    = model.getScale();
            
            glm::mat4 modelMatrix;

            ImGuizmo::RecomposeMatrixFromComponents(value_ptr(position),
                                                    value_ptr(rotation),
                                                    value_ptr(scale),
                                                    value_ptr(modelMatrix));

            ImGuizmo::Manipulate(value_ptr(cam->getViewMatrix()),
                                 value_ptr(cam->getProjectionMatrix()),
                                 m_currentOp,
                                 m_currentMode,
                                 value_ptr(modelMatrix),
                                 nullptr,
                                 m_doSnap ? &m_snap.x : nullptr);

            ImGuizmo::DecomposeMatrixToComponents(value_ptr(modelMatrix),
                                                  value_ptr(position),
                                                  value_ptr(rotation),
                                                  value_ptr(scale));

            scale = glm::max(glm::vec3(0.01f), scale);

            if (ImGuizmo::IsUsing())
            {
                model.setPosition(position);
                model.setRotationEuler(rotation);
                model.setScale(scale);
            }
        }
    }

    void GuiLayer::applyCustomColorTheme()
    {
        // cherry colors, 3 intensities
        #define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
        #define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
        #define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
        // backgrounds (@todo: complete with BG_MED, BG_LOW)
        #define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
        // text
        #define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

        auto &style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text] = TEXT(0.78f);
        style.Colors[ImGuiCol_TextDisabled] = TEXT(0.28f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_ChildWindowBg] = BG(0.58f);
        style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
        style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
        style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
        style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
        style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
        style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
        style.Colors[ImGuiCol_Header] = MED(0.76f);
        style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
        style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
        style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
        style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
        style.Colors[ImGuiCol_PlotLines] = TEXT(0.63f);
        style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = TEXT(0.63f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
        //style.Colors[ImGuiCol_Tab] = LOW(0.43f);
        //style.Colors[ImGuiCol_TabActive] = MED(1.00f);
        //style.Colors[ImGuiCol_TabHovered] = HI(1.00f);
        //style.Colors[ImGuiCol_TabUnfocused] = LOW(0.88f);
        //style.Colors[ImGuiCol_TabUnfocusedActive] = MED(0.73f);

        style.Colors[ImGuiCol_Tab] = ImLerp(style.Colors[ImGuiCol_Header], style.Colors[ImGuiCol_TitleBgActive], 0.80f);
        style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered];
        style.Colors[ImGuiCol_TabActive] = ImLerp(style.Colors[ImGuiCol_HeaderActive], style.Colors[ImGuiCol_TitleBgActive], 0.60f);
        style.Colors[ImGuiCol_TabUnfocused] = ImLerp(style.Colors[ImGuiCol_Tab], style.Colors[ImGuiCol_TitleBg], 0.80f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImLerp(style.Colors[ImGuiCol_TabActive], style.Colors[ImGuiCol_TitleBg], 0.40f);
        
        // [...]
        style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.73f);

        style.WindowPadding = ImVec2(6, 4);
        style.WindowRounding = 0.0f;
        style.FramePadding = ImVec2(5, 2);
        style.FrameRounding = 3.0f;
        style.ItemSpacing = ImVec2(7, 1);
        style.ItemInnerSpacing = ImVec2(1, 1);
        style.TouchExtraPadding = ImVec2(0, 0);
        style.IndentSpacing = 15.0f;
        style.ScrollbarSize = 12.0f;
        style.ScrollbarRounding = 16.0f;
        style.GrabMinSize = 20.0f;
        style.GrabRounding = 2.0f;

        style.WindowTitleAlign.x = 0.50f;

        style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
        style.FrameBorderSize = 0.0f;
        style.WindowBorderSize = 1.0f;
    }
}
