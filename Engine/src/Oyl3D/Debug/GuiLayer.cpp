#include "oylpch.h"
#include "GuiLayer.h"

#include "Selectable.h"

#include "App/Application.h"
#include "App/Window.h"

#include "Components/Animatable.h"
#include "Components/Collidable.h"
#include "Components/Camera.h"
#include "Components/Lights.h"
#include "Components/Misc.h"
#include "Components/Renderable.h"
#include "Components/RigidBody.h"
#include "Components/Transform.h"

#include "ECS/SystemImpl.h"

#include "Graphics/EditorCamera.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Input/Input.h"

#include "Utils/SceneToFile.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <imgui_internal.h>

#include <GLFW/glfw3.h>

static bool isTexture(const char* ext);
static bool isMesh(const char* ext);
static bool isShader(const char* ext);
static bool isMaterial(const char* ext);
static bool isScene(const char* ext);
static bool isPrefab(const char* ext);

static const char* g_entityNodeFmt = "%s\t";

static const char* g_transformGizmosName = "##ViewportGizmoSettings";
static const char* g_playPauseWindowName = "##TestPlayButton";

static const char* g_sceneWindowName     = "Scene##EditorSceneViewport";
static const char* g_gameWindowName      = "Game##EditorGameViewport";

static const char* g_hierarchyWindowName = "Hierarchy##EditorHierarchy";
static const char* g_inspectorWindowName = "Inspector##EditorInspector";

static const char* g_mainDockSpaceName = "_DockSpace";

static const char* g_numbersList[32] = {
    "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight",
    "Nine", "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen",
    "Seventeen", "Eighteen", "Nineteen", "Twenty", "Twenty One", "Twenty Two", "Twenty Three", "Twenty Four",
    "Twenty Five", "Twenty Six", "Twenty Seven", "Twenty Eight", "Twenty Nine", "Thirty", "Thirty One", "Thirty Two",
};

namespace oyl::internal
{
    void GuiLayer::init()
    {
        setupGuiLibrary();

        ImGuizmo::SetGizmoScale(2.0f);
        ImGuizmo::SetGizmoThickness(1.0f);
    }
    
    void GuiLayer::shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    
    void GuiLayer::onEnter()
    {
        scheduleSystemUpdate<EditorCameraSystem>();
        scheduleSystemUpdate<EditorRenderSystem>();

        listenForAllEvents();

        for (auto& system : m_systems)
        {
            system->setRegistry(registry);
            system->setDispatcher(m_dispatcher);
        }

        m_fileSaveTimeIt = m_fileSaveTimes.end();
        updateAssetList();
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
        //ImGui::StyleColorsDark();
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

    void GuiLayer::setupLayout()
    {
        m_consoleDockSpaceId = ImGui::GetID("_DockSpace");

        ImGui::DockBuilderRemoveNode(m_consoleDockSpaceId);
        ImGuiDockNodeFlags dockSpaceFlags = 0;
        dockSpaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockBuilderAddNode(m_consoleDockSpaceId, dockSpaceFlags);

        ImGuiID dockMain = m_consoleDockSpaceId;

        ImGuiID dockGizmoControls = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Up, 0.01f, NULL, &dockMain);
        ImGuiID dockPausePlayStep = ImGui::DockBuilderSplitNode(dockGizmoControls, ImGuiDir_Right, 0.55f, NULL, &dockGizmoControls);

        ImGuiID dockHierarchy = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.25f, NULL, &dockMain);
        ImGuiID dockInspector = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.30f, NULL, &dockMain);

        ImGuiID dockUp   = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Up, 0.1f, NULL, &dockMain);
        ImGuiID dockDown = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.15f, NULL, &dockMain);

        ImGui::DockBuilderDockWindow(g_sceneWindowName, dockMain);
        ImGui::DockBuilderDockWindow(g_gameWindowName, dockMain);

        ImGui::DockBuilderDockWindow(g_transformGizmosName, dockGizmoControls);
        ImGui::DockBuilderDockWindow(g_playPauseWindowName, dockPausePlayStep);

        ImGui::DockBuilderDockWindow(g_hierarchyWindowName, dockHierarchy);
        ImGui::DockBuilderDockWindow(g_inspectorWindowName, dockInspector);

        ImGui::DockBuilderFinish(m_consoleDockSpaceId);

        ImGui::DockBuilderGetNode(dockGizmoControls)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
        ImGui::DockBuilderGetNode(dockPausePlayStep)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
    }

    //void GuiLayer::addToCommandHistory(UniqueRef<EditorCommand>&& command)
    //{
    //    m_currentCommandPos++;
    //    m_commandHistory[m_currentCommandPos] = std::move(command);
    //}

    void GuiLayer::onExit()
    {
        for (auto& system : m_systems)
        {
            system->onExit();
        }
        
        m_systems.clear();
    }

    void GuiLayer::onUpdate()
    {
        using component::Transform;
        using component::Parent;
        using component::EntityInfo;

        auto deleteView = registry->view<entt::tag<"_delete"_hs>>();
        registry->destroy(deleteView.begin(), deleteView.end());

        // Ensure every entity has an EntityInfo component
        registry->each(
            [this](auto entity)
            {
                EntityInfo& info = registry->get_or_assign<EntityInfo>(entity);
                if (info.name.empty())
                {
                    char buf[128];
                    sprintf(buf, "Entity %d", (u32) entity);
                    info.name.assign(buf);
                }

                if (!registry->has<Transform>(entity))
                    registry->assign<Transform>(entity);
            });

        updateAssetList();
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

        if (std::ifstream guiFile("imgui.ini"); !guiFile)
            setupLayout();

        drawMenuBar();

        OYL_ASSERT(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable, "Docking should always be enabled!");
        
        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID(g_mainDockSpaceName);
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

    void GuiLayer::onGuiRender()
    {
        drawSceneHierarchy();

        drawInspector();
        
        //drawAssetList();

        drawAssetCache();

        drawSceneViewport();

        drawGameViewport();

        drawPlayPauseButtons();
    }

    bool GuiLayer::onEvent(const Event& event)
    {
        switch (event.type)
        {
            case EventType::EditorViewportHandleChanged:
            {
                auto e = event_cast<EditorViewportHandleChangedEvent>(event);
                m_editorViewportHandle = e.handle;
                return false;
            }
            case EventType::EditorEntitySelected:
            {
                auto e = event_cast<EditorEntitySelectedEvent>(event);
                m_currentSelection = Selectable(e.entity);
                return true;
            }
            case EventType::EditorCameraChanged:
            {
                auto e = event_cast<EditorCameraChangedEvent>(event);
                m_editorCamera = *e.camera;
                return false;
            }
            case EventType::ViewportHandleChanged:
            {
                auto e = event_cast<ViewportHandleChangedEvent>(event);
                m_gameViewportHandle = e.handle;
                return false;
            }
            case EventType::MousePressed:
            {
                //auto e = (MousePressedEvent) *event;
                //if (e.button == Mouse_Right)
                //{
                //    m_currentEntity = -1;
                //}
                break;
            }
            case EventType::WindowResized:
            {
                break;
            }
            case EventType::KeyReleased:
            {
                auto e = event_cast<KeyReleasedEvent>(event);
                if (e.keycode == Key::F1)
                    updateAssetList();
            }
        }
        return m_editorOverrideUpdate;
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
                    //saveSceneToFile(*Scene::current());
                    // TEMPORARY: Let name be accessed through a getter
                    registryToSceneFile(*registry, Scene::current()->m_name);

                    for (const auto& [alias, material] : Material::getCache())
                    {
                        materialToFile(material, "res/assets/materials/" + alias + ".oylmat");
                    }
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
                ImGui::SetNextWindowPos(ImGui::GetWindowSize() / 2, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                if (ImGui::Begin("Confirmation##ReloadConfirmation", 0, 
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoCollapse))
                {
                    ImGui::Text("Are you sure you want to reload the scene?");
                    ImGui::Text("You will lose any unsaved changes you've made.");

                    ImGui::Indent(ImGui::GetWindowContentRegionWidth() / 2 - 55);
                    if (ImGui::Button("Reload##ReloadConfirmationReload"))
                    {
                        for (const auto& [alias, material] : Material::getCache())
                        {
                            if (!material->getFilePath().empty() && 
                                std::fs::exists(material->getFilePath()))
                                *material = *materialFromFile(material->getFilePath());
                        }
                        
                        registryFromSceneFile(*registry, Scene::current()->m_name);
                        m_currentSelection = entt::entity(entt::null);
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
        using component::Transform;
        using component::EntityInfo;
        using component::Parent;
        
        if (ImGui::Begin(g_hierarchyWindowName, nullptr, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Dummy(ImGui::GetWindowContentRegionMax() - ImGui::GetCursorPos());
            
            if (ImGui::BeginDragDropTarget())
            {
                if (auto payload = ImGui::AcceptDragDropPayload("HierarchyEntity"))
                {
                    entt::entity child = *reinterpret_cast<entt::entity*>(payload->Data);
                    setEntityParent(child, entt::null);
                }
                ImGui::EndDragDropTarget();
            }
            
            if (ImGui::BeginPopupContextWindow("##HierarchyContextWindow"))
            {
                if (ImGui::Selectable("Add Entity##ContextWindowTest"))
                {
                    auto entityTuple = registry->create<Transform, EntityInfo>();
                    auto entity = std::get<entt::entity>(entityTuple);
                    auto& so = std::get<EntityInfo&>(entityTuple);

                    char name[128];
                    sprintf(name, "Entity %d", entity);
                    so.name.assign(name);
                }
                ImGui::EndPopup();
            }

            ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());
            
            registry->view<EntityInfo>().each(
                [&](auto entity, auto& info)
                {
                    Parent* p = registry->try_get<Parent>(entity);
                    if (!(p && registry->valid(p->parent)))
                    {
                        this->drawEntityNode(entity);
                    }
                });
        }
        ImGui::End();
    }

    void GuiLayer::drawEntityNode(entt::entity entity)
    {
        using component::EntityInfo;
        using component::Parent;
        
        auto& so = registry->get<EntityInfo>(entity);
        
        auto nodeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick |
                         ImGuiTreeNodeFlags_OpenOnArrow |
                         ImGuiTreeNodeFlags_DefaultOpen;
        
        if (entity == m_currentSelection.entity())
            nodeFlags |= ImGuiTreeNodeFlags_Selected;

        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
        auto parentView = registry->view<Parent>();
        for (auto child : parentView)
            if (parentView.get(child).parent == entity)
            {
                nodeFlags &= ~ImGuiTreeNodeFlags_Leaf;
                break;
            }

        bool treeNode = ImGui::TreeNodeEx((const void*) entity, nodeFlags, g_entityNodeFmt, so.name.c_str());
        bool clicked = ImGui::IsItemClicked(0);
        float testValue = ImGui::GetMousePos().x - ImGui::GetItemRectMin().x;
        if (treeNode)
        {
            if (ImGui::BeginPopupContextItem())
            {                
                //int flags = 0;
                //const Parent* p = registry->try_get<Parent>(entity);
                //if (!p || p->parent == entt::null)
                //{
                //    flags |= ImGuiSelectableFlags_Disabled;
                //}

                // Redundant with drag and drop
                //if (ImGui::Selectable("Clear Parent##HierarchyContextClearParent", false, flags))
                //{
                //    setEntityParent(entity, entt::null);
                //}

                if (ImGui::Selectable("Duplicate Entity##HierarchyContextDupeEntity", false))
                {                    
                    auto copy = registry->create();
                    registry->stomp(copy, entity, *registry);
                    recursiveCopy(copy, entity);

                    m_currentSelection = Selectable(entity);
                }

                if (ImGui::Selectable("Delete Entity##HierarchyContextDeleteEntity", false))
                {
                    recursiveDelete(entity);
                }
                
                ImGui::EndPopup();
            }

            if (!registry->has<entt::tag<"_delete"_hs>>(entity))
            {
                if (ImGui::BeginDragDropSource())
                {
                    ImGui::Text("%s", so.name.c_str());
                    ImGui::SetDragDropPayload("HierarchyEntity", &entity, sizeof(entt::entity));
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (auto payload = ImGui::AcceptDragDropPayload("HierarchyEntity"))
                    {
                        entt::entity child = *reinterpret_cast<entt::entity*>(payload->Data);
                        setEntityParent(child, entity);
                    }
                    ImGui::EndDragDropTarget();
                }

                parentView.each([&](auto child, Parent& p)
                {
                    if (registry->valid(p.parent) && p.parent == entity)
                    {
                        this->drawEntityNode(child);
                    }
                });
            }

            ImGui::TreePop();
        }

        if (clicked && testValue > ImGui::GetTreeNodeToLabelSpacing())
        {
            EditorEntitySelectedEvent selected;
            selected.entity = entity;
            postEvent(selected);
        }
    }

    void GuiLayer::recursiveCopy(entt::entity copy, entt::entity original)
    {
        using component::EntityInfo;
        using component::Parent;

        auto& copyEI = registry->get<EntityInfo>(copy);

        uint pos = copyEI.name.size();
        while (pos > 0 && std::isdigit(copyEI.name[pos - 1])) pos--;

        auto eiView = registry->view<EntityInfo>();
        bool isValid = false;
        do
        {
            isValid = true;
            if (pos > 0 && pos != copyEI.name.size())
            {
                int number = std::stoi(copyEI.name.substr(pos));
                copyEI.name.erase(pos);

                if (copyEI.name[pos - 1] != ' ')
                    copyEI.name.append(" ");

                copyEI.name.append(std::to_string(number + 1));
            }
            else
            {
                copyEI.name.append(" " + std::to_string(1));
            }
            for (auto entity : eiView)
            {
                if (entity != copy && eiView.get(entity).name == copyEI.name)
                {
                    isValid = false;
                    break;
                }
            }
        } while (!isValid);
        
        auto view = registry->view<Parent>();
        for (auto child : view)
        {
            if (view.get(child).parent == original)
            {
                auto e = registry->create();
                registry->stomp(e, child, *registry);
                registry->get<Parent>(e).parent = copy;
                recursiveCopy(e, child);
            }
        }
    }

    void GuiLayer::recursiveDelete(entt::entity entity)
    {
        using component::Parent;
        
        auto view = registry->view<Parent>();
        view.each([this, entity](auto child, Parent& p)
        {
            if (p.parent == entity && !registry->has<entt::tag<"_delete"_hs>>(child))
            {
                this->recursiveDelete(child);
            }
        });

        registry->assign_or_replace<entt::tag<"_delete"_hs>>(entity);

        if (m_currentSelection.entity() == entity)
            m_currentSelection = Selectable(entt::entity(entt::null));

    }

    void GuiLayer::setEntityParent(entt::entity entity, entt::entity parent)
    {
        using component::Parent;
        using component::Transform;
        
        auto& p = registry->get_or_assign<Parent>(entity);

        if (parent == p.parent) return;
        
        if (parent == entt::null && p.parent != entt::null)
        {
            auto& ct = registry->get<Transform>(entity);

            glm::mat4 tempChild = ct.getMatrixGlobal();

            glm::vec3 tComponents[3];
            ImGuizmo::DecomposeMatrixToComponents(value_ptr(tempChild),
                                                  value_ptr(tComponents[0]),
                                                  value_ptr(tComponents[1]),
                                                  value_ptr(tComponents[2]));

            tComponents[2] = max(glm::vec3(0.01f), tComponents[2]);

            ct.m_localPosition = tComponents[0];
            ct.m_localRotation = glm::quat(radians(tComponents[1]));
            ct.m_localScale = tComponents[2];
            ct.m_mirror = glm::bvec3(false);

            for (int i = 0; i < 3; i++)
                ct.m_mirror[i] = ct.m_localScale[i] < 0.0f;

            ct.m_localScale = glm::abs(ct.m_localScale);

            ct.m_isLocalDirty = true;
        }
        else
        {
            auto& ct = registry->get<Transform>(entity);
            auto& pt = registry->get<Transform>(parent);

            glm::mat4 tempChild = ct.getMatrixGlobal();
            glm::mat4 tempParent = pt.getMatrixGlobal();

            tempChild = glm::inverse(tempParent) * tempChild;

            glm::vec3 tComponents[3];
            ImGuizmo::DecomposeMatrixToComponents(value_ptr(tempChild),
                                                  value_ptr(tComponents[0]),
                                                  value_ptr(tComponents[1]),
                                                  value_ptr(tComponents[2]));

            tComponents[2] = max(glm::vec3(0.01f), tComponents[2]);

            ct.m_localPosition = tComponents[0];
            ct.m_localRotation = glm::quat(radians(tComponents[1]));
            ct.m_localScale = tComponents[2];
            ct.m_mirror = glm::bvec3(false);

            for (int i = 0; i < 3; i++)
                ct.m_mirror[i] = ct.m_localScale[i] < 0.0f;

            ct.m_localScale = glm::abs(ct.m_localScale);

            ct.m_isLocalDirty = true;
        }
        p.parent = parent;
    }

    void GuiLayer::drawInspector()
    {
        if (ImGui::Begin(g_inspectorWindowName, nullptr))
        {
            if (m_currentSelection.type() == Selectable::Type::Entity &&
                registry->valid(m_currentSelection.entity()))
            {
                drawInspectorObjectName();
                drawInspectorTransform();
                drawInspectorRenderable();
                drawInspectorGuiRenderable();
                drawInspectorCollidable();
                drawInspectorRigidBody();
                drawInspectorLightSource();
                drawInspectorCamera();
                drawInspectorAddComponent();
            }
            else if (m_currentSelection.type() == Selectable::Type::Material)
            {
                drawInspectorMaterial();
            }
        }
        ImGui::End();
    }
    
    void GuiLayer::drawInspectorObjectName()
    {
        using component::EntityInfo;

        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);

        if (registry->has<EntityInfo>(m_currentSelection.entity()) &&
            ImGui::CollapsingHeader("Object Properties##InspectorObjectProperties"))
        {
            ImGui::Indent(10);

            auto& so = registry->get<EntityInfo>(m_currentSelection.entity());

            char name[256]{ 0 };
            std::strcpy(name, so.name.c_str());

            ImGui::Text("Object Name");
            ImGui::SameLine();

            if (ImGui::InputText("##InspectorObjectNameInputField", name, 256, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                char newNameTemp[512];
                std::strcpy(newNameTemp, name);

                bool isValid = false;
                int  count = 1;
                while (!isValid)
                {
                    isValid = true;

                    auto view = registry->view<EntityInfo>();
                    for (auto entity : view)
                    {
                        if (entity == m_currentSelection.entity()) continue;

                        if (std::strcmp(newNameTemp, view.get(entity).name.c_str()) == 0)
                        {
                            sprintf(newNameTemp, "%s %d", name, count);

                            isValid = false;
                            break;
                        }
                    }
                    ++count;
                }
                so.name.assign(newNameTemp);
            }

            ImGui::Unindent(10);

            ImGui::Separator();
            ImGui::NewLine();
        }
    }
    
    void GuiLayer::drawInspectorTransform()
    {
        using component::Transform;

        if (!registry->has<Transform>(m_currentSelection.entity())) return;

        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);

        auto flags = ImGuiInputTextFlags_EnterReturnsTrue;
        
        if (ImGui::CollapsingHeader("Transform##InspectorTransform"))
        {
            ImGui::Indent(10);

            auto& transform = registry->get<Transform>(m_currentSelection.entity());

            float newWidth = ImGui::GetWindowContentRegionWidth() / 6;

            ImGui::PushItemWidth(newWidth);

            {
                bool changed = false;
                glm::vec3 position = transform.getPosition();

                const float posDragSpeed = 0.02f;
                ImGui::Text("Position");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##XPos", &position.x, posDragSpeed, 0, 0, "X");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##XPosInput", &position.x, 0, 0, "%.2f", flags);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##YPos", &position.y, posDragSpeed, 0, 0, "Y");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##YPosInput", &position.y, 0, 0, "%.2f", flags);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##ZPos", &position.z, posDragSpeed, 0, 0, "Z");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##ZPosInput", &position.z, 0, 0, "%.2f", flags);

                if (changed)
                    transform.setPosition(position);
            }
            {
                bool changed = false;
                glm::vec3 rotation = transform.getRotationEuler();

                const float rotDragSpeed = 0.5f;
                ImGui::Text("Rotation");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##XRot", &rotation.x, rotDragSpeed, 0, 0, "X");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##XRotInput", &rotation.x, 0, 0, "%.2f", flags);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##YRot", &rotation.y, rotDragSpeed, 0, 0, "Y");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##YRotInput", &rotation.y, 0, 0, "%.2f", flags);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##ZRot", &rotation.z, rotDragSpeed, 0, 0, "Z");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##ZRotInput", &rotation.z, 0, 0, "%.2f", flags);

                if (changed)
                    transform.setRotation(glm::quat(rotation));
            }
            {
                bool changed = false;
                glm::vec3 scale = transform.getScale();

                const float scaleDragSpeed = 0.02f;
                ImGui::Text("Scale");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##XSca", &scale.x, scaleDragSpeed, 0, 0, "X");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##XScaInput", &scale.x, 0, 0, "%.2f", flags);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##YSca", &scale.y, scaleDragSpeed, 0, 0, "Y");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##YScaInput", &scale.y, 0, 0, "%.2f", flags);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##ZSca", &scale.z, scaleDragSpeed, 0, 0, "Z");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##ZScaInput", &scale.z, 0, 0, "%.2f", flags);

                if (changed)
                    transform.setScale(scale);
            }
            {
                bool changed = false;
                glm::bvec3 mirror = transform.getMirror();

                ImGui::Text("Mirror");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::Checkbox("X##XMir", &mirror.x);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::Checkbox("Y##YMir", &mirror.y);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::Checkbox("Z##ZMir", &mirror.z);

                if (changed)
                    transform.setMirror(mirror);
            }

            ImGui::PopItemWidth();

            ImGui::Unindent(10);

            ImGui::Separator();
            ImGui::NewLine();
        }
    }

    void GuiLayer::drawInspectorRenderable()
    {
        using component::Renderable;

        if (!registry->has<Renderable>(m_currentSelection.entity())) return;

        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);

        bool open = ImGui::CollapsingHeader("Renderable##InspectorRenderableProperties");

        open &= !userRemoveComponent<Renderable>();

        if (open)
        {
            ImGui::Indent(10);

            auto& renderable = registry->get<Renderable>(m_currentSelection.entity());
            
            ImGui::Checkbox("Enabled", &renderable.enabled);

            ImGui::TextUnformatted("Culling Mask"); ImGui::SameLine();
            char preview[33]{ 0 };
            for (size_t i = 0; i < sizeof(preview); i++)
                preview[i] = char('0' + !!(renderable.cullingMask & 1u << (32u - i)));
            if (ImGui::BeginCombo("##RenderableCullingMaskCombo", preview))
            {
                for (int i = 0; i < 32; i++)
                {
                    if (ImGui::Selectable(g_numbersList[i], renderable.cullingMask & 1u << i, ImGuiSelectableFlags_DontClosePopups))
                        renderable.cullingMask ^= 1u << i;
                }
                ImGui::EndCombo();
            }
            
            CacheAlias currentName;
            if (currentName.assign(Mesh::getAlias(renderable.mesh)); !renderable.mesh || currentName == INVALID_ALIAS)
                currentName.assign("None");

            ImGui::TextUnformatted("Current Mesh");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX());
            if (ImGui::BeginCombo("##RenderablePropertiesCurrentMesh", currentName.c_str()))
            {
                if (ImGui::Selectable("None", renderable.mesh == nullptr))
                    renderable.mesh.reset();    
                
                const auto& meshCache = Mesh::getCache();
                for (const auto& kvp : meshCache)
                {
                    if (kvp.first != INVALID_ALIAS &&
                        ImGui::Selectable(kvp.first.c_str(), renderable.mesh == kvp.second))
                    {
                        renderable.mesh = kvp.second;
                    }
                }

                ImGui::EndCombo();
            }
            
            if (currentName.assign(Material::getAlias(renderable.material)); !renderable.material || currentName == INVALID_ALIAS)
                currentName.assign("None");
                        
            ImGui::TextUnformatted("Current Material");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX());
            if (ImGui::BeginCombo("##RenderablePropertiesCurrentMaterial", currentName.c_str()))
            {
                if (ImGui::Selectable("None", renderable.material == nullptr))
                    renderable.material.reset();
                
                const auto& meshCache = Material::getCache();
                for (const auto& kvp : meshCache)
                {
                    if (kvp.first != INVALID_ALIAS &&
                        ImGui::Selectable(kvp.first.c_str(), renderable.material == kvp.second))
                    {
                        renderable.material = kvp.second;
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::Unindent(10);

            ImGui::Separator();
            ImGui::NewLine();
        }
    }

    void GuiLayer::drawInspectorGuiRenderable()
    {
        using component::GuiRenderable;

        if (!registry->has<GuiRenderable>(m_currentSelection.entity())) return;

        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);

        bool open = ImGui::CollapsingHeader("GuiRenderable##InspectorGuiRenderableProperties");

        open &= !userRemoveComponent<GuiRenderable>();

        if (open)
        {
            ImGui::Indent(10);

            auto& gui = registry->get<GuiRenderable>(m_currentSelection.entity());

            ImGui::Checkbox("Enabled##GuiRenderableEnabled", &gui.enabled);

            ImGui::TextUnformatted("Culling Mask"); ImGui::SameLine();
            char preview[33]{ 0 };
            for (size_t i = 0; i < sizeof(preview); i++)
                preview[i] = char('0' + !!(gui.cullingMask & 1u << (32u - i)));
            if (ImGui::BeginCombo("##GuiRenderableCullingMaskCombo", preview))
            {
                for (int i = 0; i < 32; i++)
                {
                    if (ImGui::Selectable(g_numbersList[i], gui.cullingMask & 1u << i, ImGuiSelectableFlags_DontClosePopups))
                        gui.cullingMask ^= 1u << i;
                }
                ImGui::EndCombo();
            }

            CacheAlias currentName;
            if (currentName.assign(Texture2D::getAlias(gui.texture)); !gui.texture || currentName == INVALID_ALIAS)
                currentName.assign("None");
            
            ImGui::TextUnformatted("Current Texture");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX());
            if (ImGui::BeginCombo("##GuiRenderablePropertiesCurrentTexture", currentName.c_str()))
            {
                if (ImGui::Selectable("None", !gui.texture || currentName == INVALID_ALIAS))
                    gui.texture.reset();

                const auto& textureCache = Texture2D::getCache();
                for (const auto& [alias, texture] : textureCache)
                {
                    if (alias != INVALID_ALIAS &&
                        ImGui::Selectable(alias.c_str(), gui.texture == texture))
                    {
                        gui.texture = texture;
                    }
                }

                ImGui::EndCombo();
            }

            int flags = ImGuiInputTextFlags_EnterReturnsTrue;
            const float posDragSpeed = 0.02f;
            float newWidth = ImGui::GetWindowContentRegionWidth() / 6;

            ImGui::TextUnformatted("Clipping");
            {
                ImGui::PushItemWidth(newWidth);
                ImGui::Indent(10);
            
                ImGui::TextUnformatted("Lower");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##LowerClippingX", &gui.lowerClipping.x, posDragSpeed, 0, 1.0f, "X");
                ImGui::SameLine();
                ImGui::InputFloat("##LowerClippingInputX", &gui.lowerClipping.x, 0, 0, "%.2f", flags);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##LowerClippingY", &gui.lowerClipping.y, posDragSpeed, 0, 1.0f, "Y");
                ImGui::SameLine();
                ImGui::InputFloat("##LowerClippingInputY", &gui.lowerClipping.y, 0, 0, "%.2f", flags);

                ImGui::TextUnformatted("Upper");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##UpperClippingX", &gui.upperClipping.x, posDragSpeed, 0, 1.0f, "X");
                ImGui::SameLine();
                ImGui::InputFloat("##UpperClippingInputX", &gui.upperClipping.x, 0, 0, "%.2f", flags);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                ImGui::DragFloat("##UpperClippingY", &gui.upperClipping.y, posDragSpeed, 0, 1.0f, "Y");
                ImGui::SameLine();
                ImGui::InputFloat("##UpperClippingInputY", &gui.upperClipping.y, 0, 0, "%.2f", flags);

                ImGui::Unindent(10);
                ImGui::PopItemWidth();
            }
            
            ImGui::Unindent(10);

            ImGui::Separator();
            ImGui::NewLine();
        }
    }

    void GuiLayer::drawInspectorCollidable()
    {
        using component::Collidable;

        if (!registry->has<Collidable>(m_currentSelection.entity())) return;

        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);

        auto flags = ImGuiInputTextFlags_EnterReturnsTrue;

        bool open = ImGui::CollapsingHeader("Collidable##InspectorColliderProperties");

        open &= !userRemoveComponent<Collidable>();

        if (open)
        {
            ImGui::Indent(10);

            auto& collider = registry->get<Collidable>(m_currentSelection.entity());
            int count = 0;
            char shapeID[512];
            char temp[128];
            for (auto& shape : collider)
            {
                if (shape.getType() == ColliderType::Box)
                    strcpy(shapeID, "Box");
                else if (shape.getType() == ColliderType::Sphere)
                    strcpy(shapeID, "Sphere");
                else if (shape.getType() == ColliderType::Capsule)
                    strcpy(shapeID, "Capsule");
                else if (shape.getType() == ColliderType::Cylinder)
                    strcpy(shapeID, "Cylinder");
                //else if (shape.getType() == ColliderType::Mesh)
                //    strcpy(shapeID, "Mesh");

                sprintf(shapeID, "%s##ColliderPropertiesShape%d", shapeID, count);

                ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader(shapeID))
                {
                    ImGui::Indent(10);

                    sprintf(temp, "##ColliderChangeShape%d", count);
                    if (ImGui::BeginCombo(temp, "Set Type"))
                    {
                        sprintf(temp, "Box##Box%d", count);
                        if (ImGui::Selectable(temp))
                            shape.setType(ColliderType::Box);
                        sprintf(temp, "Sphere##Sphere%d", count);
                        if (ImGui::Selectable(temp))
                            shape.setType(ColliderType::Sphere);
                        sprintf(temp, "Capsule##Capsule%d", count);
                        if (ImGui::Selectable(temp))
                            shape.setType(ColliderType::Capsule);
                        sprintf(temp, "Cylinder##Cylinder%d", count);
                        if (ImGui::Selectable(temp))
                            shape.setType(ColliderType::Cylinder);
                        //sprintf(temp, "Mesh##Mesh%d", count);
                        //if (ImGui::Selectable(temp))
                        //    shape.setType(ColliderType::Mesh);

                        ImGui::EndCombo();
                    }   

                    float newWidth = ImGui::GetWindowContentRegionWidth() / 6;

                    ImGui::PushItemWidth(newWidth);
                    
                    {
                        glm::vec3 position = shape.box.getCenter();
                        const float posDragSpeed = 0.02f;
                        ImGui::Text("Center");
                        ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                        ImGui::SetNextItemWidth(15);
                        sprintf(temp, "##XCenter%d", count);
                        ImGui::DragFloat(temp, &position.x, posDragSpeed, 0, 0, "X");
                        ImGui::SameLine();
                        sprintf(temp, "##XCenterInput%d", count);
                        ImGui::InputFloat(temp, &position.x, 0, 0, "%.2f", flags);
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(15);
                        sprintf(temp, "##YCenter%d", count);
                        ImGui::DragFloat(temp, &position.y, posDragSpeed, 0, 0, "Y");
                        ImGui::SameLine();
                        sprintf(temp, "##YCenterInput%d", count);
                        ImGui::InputFloat(temp, &position.y, 0, 0, "%.2f", flags);
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(15);
                        sprintf(temp, "##ZCenter%d", count);
                        ImGui::DragFloat(temp, &position.z, posDragSpeed, 0, 0, "Z");
                        ImGui::SameLine();
                        sprintf(temp, "##ZCenterInput%d", count);
                        ImGui::InputFloat(temp, &position.z, 0, 0, "%.2f", flags);

                        if (position != shape.box.getCenter())
                            shape.box.setCenter(position);
                    }

                    switch (shape.getType())
                    {
                        case ColliderType::Box:
                        {
                            glm::vec3 size = shape.box.getSize();
                            const float posDragSpeed = 0.02f;
                            ImGui::Text("Size");
                            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                            ImGui::SetNextItemWidth(15);
                            sprintf(temp, "##XSize%d", count);
                            ImGui::DragFloat(temp, &size.x, posDragSpeed, 0, 0, "X");
                            ImGui::SameLine();
                            sprintf(temp, "##XSizeInput%d", count);
                            ImGui::InputFloat(temp, &size.x, 0, 0, "%.2f", flags);
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(15);
                            sprintf(temp, "##YSize%d", count);
                            ImGui::DragFloat(temp, &size.y, posDragSpeed, 0, 0, "Y");
                            ImGui::SameLine();
                            sprintf(temp, "##YSizeInput%d", count);
                            ImGui::InputFloat(temp, &size.y, 0, 0, "%.2f", flags);
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(15);
                            sprintf(temp, "##ZSize%d", count);
                            ImGui::DragFloat(temp, &size.z, posDragSpeed, 0, 0, "Z");
                            ImGui::SameLine();
                            sprintf(temp, "##ZSizeInput%d", count);
                            ImGui::InputFloat(temp, &size.z, 0, 0, "%.2f", flags);

                            if (size != shape.box.getSize())
                                shape.box.setSize(size);
                            
                            break;
                        }
                        case ColliderType::Sphere:
                        {
                            float radius = shape.sphere.getRadius();
                            const float posDragSpeed = 0.01f;
                            ImGui::Text("Radius");
                            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                            ImGui::SetNextItemWidth(15);
                            sprintf(temp, "##Radius%d", count);
                            ImGui::DragFloat(temp, &radius, posDragSpeed, 0, 0, "R");
                            ImGui::SameLine();
                            sprintf(temp, "##RadiusInput%d", count);
                            ImGui::InputFloat(temp, &radius, 0, 0, "%.2f", flags);

                            radius = glm::max(radius, 0.1f);
                            if (radius != shape.sphere.getRadius())
                                shape.sphere.setRadius(radius);

                            break;
                        }
                        case ColliderType::Capsule:
                        {
                            float radius = shape.capsule.getRadius();
                            const float posDragSpeed = 0.01f;
                            ImGui::TextUnformatted("Radius");
                            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                            ImGui::SetNextItemWidth(15);
                            sprintf(temp, "##Radius%d", count);
                            ImGui::DragFloat(temp, &radius, posDragSpeed, 0, 0, "R");
                            ImGui::SameLine();
                            sprintf(temp, "##RadiusInput%d", count);
                            ImGui::InputFloat(temp, &radius, 0, 0, "%.2f", flags);

                            radius = glm::max(radius, 0.1f);
                            if (radius != shape.capsule.getRadius())
                                shape.capsule.setRadius(radius);
                            
                            float height = shape.capsule.getHeight();
                            ImGui::TextUnformatted("Height");
                            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
                            ImGui::SetNextItemWidth(15);
                            sprintf(temp, "##Height%d", count);
                            ImGui::DragFloat(temp, &height, posDragSpeed, 0, 0, "R");
                            ImGui::SameLine();
                            sprintf(temp, "##HeightInput%d", count);
                            ImGui::InputFloat(temp, &height, 0, 0, "%.2f", flags);

                            height = glm::max(height, 0.1f);
                            if (height != shape.capsule.getHeight())
                                shape.capsule.setHeight(height);

                            int direction = static_cast<int>(shape.capsule.getDirection());
                            ImGui::TextUnformatted("Direction"); ImGui::SameLine();
                            ImGui::RadioButton("X##DirectionRadioX", &direction, static_cast<int>(Direction::X_AXIS));
                            ImGui::SameLine();
                            ImGui::RadioButton("Y##DirectionRadioY", &direction, static_cast<int>(Direction::Y_AXIS));
                            ImGui::SameLine();
                            ImGui::RadioButton("Z##DirectionRadioZ", &direction, static_cast<int>(Direction::Z_AXIS));
                            shape.capsule.setDirection(static_cast<Direction>(direction));
                            
                            break;
                        }
                    }

                    ImGui::Unindent(10);

                    ImGui::PopItemWidth();
                }

                ++count;
            }

            ImGui::NewLine();

            if (ImGui::BeginCombo("##InspectorCollidableAddShape", "Add Shape", ImGuiComboFlags_NoArrowButton))
            {
                if (ImGui::Selectable("Box"))
                    collider.pushShape(ColliderType::Box);
                if (ImGui::Selectable("Sphere"))
                    collider.pushShape(ColliderType::Sphere);
                if (ImGui::Selectable("Capsule"))
                    collider.pushShape(ColliderType::Capsule);
                if (ImGui::Selectable("Cylinder"))
                    collider.pushShape(ColliderType::Cylinder);
                if (ImGui::Selectable("Mesh"))
                    collider.pushShape(ColliderType::Sphere);

                ImGui::EndCombo();
            }

            ImGui::Unindent(10);

            ImGui::Separator();
            ImGui::NewLine();
        }
    }

    void GuiLayer::drawInspectorRigidBody()
    {
        using component::RigidBody;

        if (!registry->has<RigidBody>(m_currentSelection.entity()))
            return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);

        bool open = ImGui::CollapsingHeader("RigidBody##InspectorRigidBody");

        open &= !userRemoveComponent<RigidBody>();
        
        if (open)
        {
            ImGui::Indent(10);

            auto& rb = registry->get<RigidBody>(m_currentSelection.entity());

            // TODO: Make boxes relative to right side of window like transforms are
            
            // TODO: Add more property checkboxes

            auto doCheckbox = [&](const char* name, RigidBody::Property prop)
            {
                ImGui::TextUnformatted(name);
                ImGui::SameLine();
                bool ret = rb.getProperty(prop);
                char id[128];
                sprintf_s(id, sizeof(id), "##InspectorRigidBody%sCheckbox", name);
                ImGui::Checkbox(id, &ret);
                if (ret != rb.getProperty(prop))
                    rb.setProperties(prop, ret);
                return ret;
            };

            bool useGravity = doCheckbox("Use Gravity", RigidBody::USE_GRAVITY);
            bool isKinematic = doCheckbox("Is Kinematic", RigidBody::IS_KINEMATIC);
            bool doCollisions = doCheckbox("Detect Collisions", RigidBody::DETECT_COLLISIONS);

            glm::bvec3 isFrozen = {
                rb.getProperty(RigidBody::FREEZE_ROTATION_X),
                rb.getProperty(RigidBody::FREEZE_ROTATION_Y),
                rb.getProperty(RigidBody::FREEZE_ROTATION_Z)
            };

            ImGui::TextUnformatted("Freeze Rotation");
            ImGui::SameLine();
            ImGui::Checkbox("X##InspectorRigidBodyFreezeRotXCheckbox", &isFrozen.x);
            ImGui::SameLine();
            ImGui::Checkbox("Y##InspectorRigidBodyFreezeRotYCheckbox", &isFrozen.y);
            ImGui::SameLine();
            ImGui::Checkbox("Z##InspectorRigidBodyFreezeRotZCheckbox", &isFrozen.z);

            rb.setProperties(RigidBody::FREEZE_ROTATION_X, isFrozen.x);
            rb.setProperties(RigidBody::FREEZE_ROTATION_Y, isFrozen.y);
            rb.setProperties(RigidBody::FREEZE_ROTATION_Z, isFrozen.z);

            float mass = isKinematic ? 0.0f : rb.getMass();
            ImGui::Text("Mass");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##DragMass", &mass, isKinematic ? 0.0f : 0.02f, 0.0f, 1000.0f, "M");
            ImGui::SameLine();
            ImGui::InputFloat("##MassInput", &mass, 0, 0, "%.2f", isKinematic ? ImGuiInputTextFlags_ReadOnly : 0);
            if (!isKinematic && mass != rb.getMass())
                rb.setMass(mass);

            float friction = rb.getFriction();
            ImGui::Text("Friction");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(15);
            ImGui::DragFloat("##DragFriction", &friction, 0.02f, 0.0f, 1000.0f, "F");
            ImGui::SameLine();
            ImGui::InputFloat("##FrictionInput", &friction, 0, 0, "%.2f");
            if (friction != rb.getFriction())
                rb.setFriction(friction);

            if (ImGui::CollapsingHeader("Simulation Values##InspectorRigidBodySimValues"))
            {
                ImGui::Indent(10);

                ImGui::Text("Nothing to see here yet :)");

                ImGui::Unindent(10);
            }
            
            ImGui::Unindent(10);

            ImGui::Separator();
            ImGui::NewLine();
        }
    }

    void GuiLayer::drawInspectorLightSource()
    {
        // TEMPORARY: Change when adding LightSource component
        using component::PointLight;

        if (!registry->has<PointLight>(m_currentSelection.entity()))
            return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);

        bool open = ImGui::CollapsingHeader("Light Source##InspectorLightSource");

        open &= !userRemoveComponent<PointLight>();

        if (open)
        {
            ImGui::Indent(10);

            auto& pl = registry->get<PointLight>(m_currentSelection.entity());

            ImGui::ColorEdit3("Ambient##LightSourceAmbient",  value_ptr(pl.ambient));
            ImGui::ColorEdit3("Diffuse##LightSourceAmbient",  value_ptr(pl.diffuse));
            ImGui::ColorEdit3("Specular##LightSourceAmbient", value_ptr(pl.specular));

            ImGui::Unindent(10);

            ImGui::Separator();
            ImGui::NewLine();
        }
    }

    void GuiLayer::drawInspectorCamera()
    {
        // TEMPORARY: Change when adding LightSource component
        using component::Camera;

        if (!registry->has<Camera>(m_currentSelection.entity()))
            return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);

        bool open = ImGui::CollapsingHeader("Camera##InspectorCamera");

        open &= !userRemoveComponent<Camera>();

        if (open)
        {
            ImGui::Indent(10);

            auto& camera = registry->get<Camera>(m_currentSelection.entity());

            ImGui::Checkbox("Enabled##CameraEnabled", &camera.enabled);

            ImGui::TextUnformatted("Culling Mask"); ImGui::SameLine();
            char preview[33]{ 0 };
            for (size_t i = 0; i < sizeof(preview); i++)
                preview[i] = char('0' + !!(camera.cullingMask & 1u << (32u - i)));
            if (ImGui::BeginCombo("##CameraCullingMaskCombo", preview))
            {
                for (int i = 0; i < 32; i++)
                {
                    if (ImGui::Selectable(g_numbersList[i], camera.cullingMask & 1u << i, ImGuiSelectableFlags_DontClosePopups))
                        camera.cullingMask ^= 1u << i;
                }
                ImGui::EndCombo();
            }

            // TEMPORARY:
            ImGui::TextUnformatted("Player Number");
            if (ImGui::BeginCombo("##CameraPlayerNumber", g_numbersList[static_cast<uint>(camera.player)]))
            {
                for (uint i = 0; i <= static_cast<uint>(PlayerNumber::Four); i++)
                {
                    if (ImGui::Selectable(g_numbersList[i], i == static_cast<uint>(camera.player)))
                    {
                        bool didSwap = false;
                        registry->view<Camera>().each([&](Camera& a_camera)
                        {
                            if (a_camera.player == static_cast<PlayerNumber>(i))
                                std::swap(a_camera.player, camera.player), didSwap = true;
                        });
                        if (!didSwap)
                            camera.player = static_cast<PlayerNumber>(i);
                    }
                }
                ImGui::EndCombo();
            }
            {
                bool changed = false;
                float fov = camera.fov();
                ImGui::TextUnformatted("Field of View");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##DragFov", &fov, 0.2f, 0.0f, 1000.0f, "F");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##FovInput", &fov, 0, 0, "%.2f");
                if (changed)
                    camera.fov(fov);
            }
            {
                bool changed = false;
                float nearClipping = camera.nearClipping();
                ImGui::TextUnformatted("Near Clipping Plane");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##DragNearClipping", &nearClipping, 0.2f, 0.0f, 1000.0f, "Z");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##NearClippingInput", &nearClipping, 0, 0, "%.2f");
                if (changed)
                    camera.nearClipping(nearClipping);
            }
            {
                bool changed = false;
                float farClipping = camera.farClipping();
                ImGui::TextUnformatted("Field of View");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(15);
                changed |= ImGui::DragFloat("##FarClippingFov", &farClipping, 0.2f, 0.0f, 1000.0f, "Z");
                ImGui::SameLine();
                changed |= ImGui::InputFloat("##FarClippingInput", &farClipping, 0, 0, "%.2f");
                if (changed)
                    camera.farClipping(farClipping);
            }

            // TODO: Upper and Lower Coords

            ImGui::Unindent(10);

            ImGui::Separator();
            ImGui::NewLine();
        }
    }

    void GuiLayer::drawInspectorAddComponent()
    {
        using component::Collidable;
        using component::GuiRenderable;
        using component::PointLight;
        using component::Renderable;
        using component::RigidBody;
        using component::Camera;
        
        if (ImGui::BeginCombo("##InspectorAddComponent", "Add Component", ImGuiComboFlags_NoArrowButton))
        {
            auto entity = m_currentSelection.entity();
            
            if (!registry->has<Renderable>(entity) &&
                ImGui::Selectable("Renderable"))
                registry->assign<Renderable>(entity);

            if (!registry->has<GuiRenderable>(entity) &&
                ImGui::Selectable("GuiRenderable"))
                registry->assign<GuiRenderable>(entity);
            
            if (!registry->has<Collidable>(entity) &&
                ImGui::Selectable("Collider"))
                registry->assign<Collidable>(entity);
            
            if (!registry->has<RigidBody>(entity) &&
                ImGui::Selectable("RigidBody"))
                registry->assign<RigidBody>(entity);

            if (!registry->has<PointLight>(entity) &&
                ImGui::Selectable("Point Light"))
                registry->assign<PointLight>(entity);

            int camFlags = 0;
            if (registry->size<Camera>() >= 4)
                camFlags |= ImGuiSelectableFlags_Disabled;
            if (!registry->has<Camera>(entity) &&
                ImGui::Selectable("Camera", false, camFlags))
            {
                auto& cam = registry->assign<Camera>(entity);
                bool valid = true;
                do
                {
                    valid = true;  
                    registry->view<Camera>().each([&](entt::entity a_entity, auto& a_camera)
                    {
                        if (a_entity != entity && cam.player == a_camera.player)
                        {
                            cam.player = static_cast<PlayerNumber>(static_cast<uint>(cam.player) + 1u);
                            valid = false;
                        }
                    });
                } while (!valid && cam.player != PlayerNumber::Four);
            }

            ImGui::EndCombo();
        }
    }

    static void _setTextureProfile(const Ref<Texture2D>& a_texture)
    {
        if (!a_texture) return;

        ImGui::Indent(10);
        
        bool isRGB = a_texture->getProfile() == TextureProfile::RGB;
        const char* profilePreview = isRGB ? "RGB" : "sRGB";
        char profileID[256];
        sprintf_s(profileID, 256, "Texture Profile##%sTexProfile", a_texture->getFilePath().c_str());
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(profileID, profilePreview))
        {
            if (ImGui::Selectable("RGB", isRGB) && !isRGB)
                a_texture->setProfile(TextureProfile::RGB);
            if (ImGui::Selectable("sRGB", !isRGB) && isRGB)
                a_texture->setProfile(TextureProfile::SRGB);

            ImGui::EndCombo();
        }
        switch (a_texture->getFilter())
        {
            case TextureFilter::Nearest: profilePreview = "Nearest"; break;
            case TextureFilter::Linear: profilePreview = "Linear"; break;
        }
        sprintf_s(profileID, 256, "Texture Filtering##%sTexFilter", a_texture->getFilePath().c_str());
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(profileID, profilePreview))
        {
            if (ImGui::Selectable("Nearest", a_texture->getFilter() == TextureFilter::Nearest))
                a_texture->setFilter(TextureFilter::Nearest);
            if (ImGui::Selectable("Linear", a_texture->getFilter() == TextureFilter::Linear))
                a_texture->setFilter(TextureFilter::Linear);

            ImGui::EndCombo();
        }
        switch (a_texture->getWrap())
        {
            case TextureWrap::Repeat: profilePreview = "Repeat"; break;
            case TextureWrap::Mirror: profilePreview = "Mirror"; break;
            case TextureWrap::ClampToEdge: profilePreview = "Clamp to Edge"; break;
            case TextureWrap::ClampToBorder: profilePreview = "Clamp to Border"; break;
        }
        sprintf_s(profileID, 256, "Texture Wrapping##%sTexWrap", a_texture->getFilePath().c_str());
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(profileID, profilePreview))
        {
            if (ImGui::Selectable("Repeat", a_texture->getWrap() == TextureWrap::Repeat))
                a_texture->setWrap(TextureWrap::Repeat);
            if (ImGui::Selectable("Mirror", a_texture->getWrap() == TextureWrap::Mirror))
                a_texture->setWrap(TextureWrap::Mirror);
            if (ImGui::Selectable("Clamp to Edge", a_texture->getWrap() == TextureWrap::ClampToEdge))
                a_texture->setWrap(TextureWrap::ClampToEdge);
            if (ImGui::Selectable("Clamp to Border", a_texture->getWrap() == TextureWrap::ClampToBorder))
                a_texture->setWrap(TextureWrap::ClampToBorder);
            
            ImGui::EndCombo();
        }

        ImGui::Unindent(10);
    }

    void GuiLayer::drawInspectorMaterial()
    {
        auto& material = m_currentSelection.material();

        std::string tempAlias = Shader::getAlias(material->shader);
        if (tempAlias == INVALID_ALIAS) tempAlias = "None";
        if (ImGui::BeginCombo("Shader", tempAlias.c_str()))
        {
            for (const auto& [alias, shader] : Shader::getCache())
                if (ImGui::Selectable(alias.c_str(), shader == material->shader))
                    material->shader = shader;

            ImGui::EndCombo();
        }
        ImGui::NewLine();

        auto _selectTexture = [&tempAlias](Ref<Texture2D>& a_texture, const char* type)
        {
            tempAlias = Texture2D::getAlias(a_texture);
            if (tempAlias == INVALID_ALIAS) tempAlias = "None";
            if (ImGui::BeginCombo(type, tempAlias.c_str()))
            {
                if (ImGui::Selectable("None", a_texture == nullptr))
                    a_texture = nullptr;
                for (const auto& [alias, texture] : Texture2D::getCache())
                    if (ImGui::Selectable(alias.c_str(), texture == a_texture))
                        a_texture = texture;

                ImGui::EndCombo();
            }
        };
        
        _selectTexture(material->albedoMap, "Albedo Map");
        _setTextureProfile(material->albedoMap);
        ImGui::NewLine();

        _selectTexture(material->specularMap, "Specular Map");
        _setTextureProfile(material->specularMap);
        ImGui::NewLine();

        _selectTexture(material->normalMap, "Normal Map");
        _setTextureProfile(material->normalMap);
        ImGui::NewLine();

        _selectTexture(material->emissionMap, "Emission Map");
        _setTextureProfile(material->emissionMap);
        ImGui::NewLine();

        auto flags = ImGuiInputTextFlags_EnterReturnsTrue;

        glm::vec2& tiling = material->mainTextureProps.tiling;
        glm::vec2& offset = material->mainTextureProps.offset;

        const float posDragSpeed = 0.02f;
        float newWidth = ImGui::GetWindowContentRegionWidth() / 6;
        ImGui::PushItemWidth(newWidth);

        ImGui::TextUnformatted("Tiling");
        ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
        ImGui::SetNextItemWidth(15);
        ImGui::DragFloat("##TilingX", &tiling.x, posDragSpeed, 0, 0, "X");
        ImGui::SameLine();
        ImGui::InputFloat("##TilingInputX", &tiling.x, 0, 0, "%.2f", flags);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(15);
        ImGui::DragFloat("##TilingY", &tiling.y, posDragSpeed, 0, 0, "Y");
        ImGui::SameLine();
        ImGui::InputFloat("##TilingInputY", &tiling.y, 0, 0, "%.2f", flags);

        ImGui::TextUnformatted("Offset");
        ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (15 * 3 + newWidth * 3 + 27));
        ImGui::SetNextItemWidth(15);
        ImGui::DragFloat("##OffsetX", &offset.x, posDragSpeed, 0, 0, "X");
        ImGui::SameLine();
        ImGui::InputFloat("##OffsetInputX", &offset.x, 0, 0, "%.2f", flags);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(15);
        ImGui::DragFloat("##OffsetY", &offset.y, posDragSpeed, 0, 0, "Y");
        ImGui::SameLine();
        ImGui::InputFloat("##OffsetInputY", &offset.y, 0, 0, "%.2f", flags);

        ImGui::PopItemWidth();
    }

    void GuiLayer::drawAssetList()
    {   
        if (ImGui::Begin("Assets##AssetListMainWindow"))
        {
            uint flags = (ImGuiTableFlags_Resizable |
                          ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_BordersVInner |
                          ImGuiTableFlags_BordersHInner |
                          ImGuiTableFlags_ScrollY |
                          ImGuiTableFlags_ScrollFreezeTopRow);

            if (ImGui::BeginTable("##AssetTable", 2, flags))
            {
                uint cFlags = ImGuiTableColumnFlags_WidthStretch |
                              ImGuiTableColumnFlags_NoHide;
                ImGui::TableSetupColumn("Name", cFlags);
                ImGui::TableSetupColumn("Type", cFlags);
                ImGui::TableAutoHeaders();

                drawAssetNode(std::fs::directory_iterator("res"));
                
                ImGui::EndTable();
            }
        }
        
        ImGui::End();
    }
    
    void GuiLayer::drawAssetNode(const std::fs::directory_iterator& dir)
    {
        const char* typeName = nullptr;
        
        uint flags = ImGuiTreeNodeFlags_SpanFullWidth;
        for (const auto& dirEntry : dir)
        {
            ImGui::TableNextRow();

            std::string name = dirEntry.path().filename().string();
            const char* ext = name.c_str() + name.find_last_of('.');

            if (dirEntry.is_directory())
            {
                flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

                typeName = "Folder";
            } else
            {
                flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

                if      (isTexture(ext))  typeName = "Texture";
                else if (isMesh(ext))     typeName = "Mesh";
                else if (isShader(ext))   typeName = "Shader";
                else if (isMaterial(ext)) typeName = "Material";
                else if (isScene(ext))    typeName = "Scene";
                else if (isPrefab(ext))   typeName = "Prefab";
            }
            
            bool open = ImGui::TreeNodeEx(name.c_str(), flags);

            //if (ImGui::BeginDragDropSource())
            //{
            //    
            //    ImGui::SetDragDropPayload("AssetInList", , sizeof(Selectable));
            //    ImGui::EndDragDropSource();
            //}
            
            //if (ImGui::BeginPopupContextItem())
            //{
            //    if (ImGui::BeginMenu("New"))
            //    {
            //        if (ImGui::MenuItem("Material"))
            //        {
            //            ImGui::TextUnformatted("Hehe");
            //        }
            //        if (ImGui::MenuItem("Prefab"))
            //        {
            //            ImGui::TextUnformatted("Haha");
            //        }
            //        ImGui::EndMenu();
            //    }
            //    ImGui::EndPopup();
            //}
            
            ImGui::TableNextCell();
            if (dirEntry.is_directory())
                ImGui::TextDisabled("Folder");
            else
                ImGui::TextUnformatted(typeName != nullptr ? typeName : "Unknown");

            if (open)
            {
                drawAssetNode(std::fs::directory_iterator(dirEntry));
                ImGui::TreePop();
            }
        }
    }

    // TODO: Update algorithm to check recursive driectories and cached for any change to update file 
    void GuiLayer::updateAssetList()
    {
        if (m_fileSaveTimeIt != m_fileSaveTimes.end())
        {
            bool                    stillExists      = true;
            std::fs::file_time_type lastWriteTimeNew = {};

            do
            {
                stillExists = true;
                auto path   = std::fs::path(m_fileSaveTimeIt->first);
                if (exists(path))
                {
                    lastWriteTimeNew = last_write_time(path);
                }
                else
                {
                    auto pathStr = path.string();
                    
                    m_fileSaveTimeIt = m_fileSaveTimes.erase(m_fileSaveTimeIt);
                    stillExists      = false;

                    std::string ext = path.extension().string();

                    if (isTexture(ext.c_str()))
                        for (const auto& [alias, texture] : Texture2D::getCache())
                            if (pathStr == texture->getFilePath())
                                Texture2D::discard(alias);

                    if (isMesh(ext.c_str()))
                        for (const auto& [alias, mesh] : Mesh::getCache())
                            if (pathStr == mesh->getFilePath())
                                Mesh::discard(alias);

                    if (isMaterial(ext.c_str()))
                        for (const auto& [alias, material] : Material::getCache())
                            if (pathStr == material->getFilePath())
                                Material::discard(alias);
                }
            } while (!stillExists);
            
            if (lastWriteTimeNew != std::fs::file_time_type{} && 
                lastWriteTimeNew != m_fileSaveTimeIt->second)
            {
                m_fileSaveTimeIt->second = lastWriteTimeNew;
                
                const char* ext =
                    m_fileSaveTimeIt->first.c_str() + m_fileSaveTimeIt->first.find_last_of('.');

                if (isShader(ext))
                    for (const auto& kvp : Shader::getCache())
                        for (const auto& shader : kvp.second->getShaderInfos())
                        {
                            //auto pair = std::make_pair<const ShaderInfo&, const Ref<Shader>&>(shader, kvp.second);
                            if (updateAsset(shader.filename,
                                            [](void* data)
                                            {
                                                auto& shaderRef = *reinterpret_cast<Ref<Shader>*>(data);
                                                //auto  shader    = Shader::create(shaderRef->getShaderInfos());
                                                //if (!shader->getShaderInfos().empty()) *shaderRef = *shader;
                                                shaderRef->load(shaderRef->getShaderInfos());
                                            }, (void*) &kvp.second))
                            {
                                break;
                            }
                        }

                if (isMaterial(ext))
                    for (auto& [alias, material] : Material::getCache())
                        if (updateAsset(material->getFilePath(),
                                        [](void* data)
                                        {
                                            auto& materialRef = *reinterpret_cast<Ref<Material>*>(data);
                                            auto  mat         = Material::create(materialRef->getFilePath());
                                            if (mat) *materialRef = *mat;
                                        }, (void*) &material))
                        {
                            break;
                        }

                if (isMesh(ext))
                    for (auto& [alias, mesh] : Mesh::getCache())
                        if (updateAsset(mesh->getFilePath(),
                                        [](void* data)
                                        {
                                            auto& meshRef = *reinterpret_cast<Ref<Mesh>*>(data);
                                            auto  mesh    = Mesh::create(meshRef->getFilePath());
                                            if (mesh != Mesh::get(INVALID_ALIAS)) *meshRef = *mesh;
                                        }, (void*) &mesh))
                        {
                            break;
                        }

                if (isTexture(ext))
                    for (auto& [alias, texture] : Texture2D::getCache())
                        if (updateAsset(texture->getFilePath(),
                                        [](void* data)
                                        {
                                            auto& textureRef = *reinterpret_cast<Ref<Texture2D>*>(data);
                                            auto  tex        = Texture2D::create(textureRef->getFilePath());
                                            if (tex->isLoaded()) *textureRef = *tex;
                                        }, (void*) &texture))
                        {
                            break;
                        }


            }

            ++m_fileSaveTimeIt;
        }
        else
        {
            std::fs::recursive_directory_iterator res("res");
            for (const auto& dirEntry : res)
            {   
                auto        relPath    = relative(dirEntry.path());
                std::string relPathStr = relPath.string();
                if (m_fileSaveTimes.find(relPathStr) == m_fileSaveTimes.end())
                {
                    m_fileSaveTimes[relPathStr] = last_write_time(relPath);
                    const char* ext             = relPathStr.c_str() + relPathStr.find_last_of('.');

                    if (isTexture(ext))  Texture2D::cache(relPathStr);
                    if (isMesh(ext))     Mesh::cache(relPathStr);
                    if (isMaterial(ext)) Material::cache(relPathStr);
                }
            }

            m_fileSaveTimeIt = m_fileSaveTimes.begin();

            // Helper function, return true if was not present in the list, return false otherwise
            auto addToFileSaveTimes = [this](const std::string& filename)
            {
                if (filename.empty()) return;
                auto relPath    = std::fs::relative(filename);
                auto relPathStr = relPath.string();
                if (m_fileSaveTimes.find(relPathStr) == m_fileSaveTimes.end())
                    m_fileSaveTimes[relPathStr] = last_write_time(relPath);
            };
            
            // If at the end, iterate through all of the assets and
            // add them to the list if they're not already 
            for (const auto& [alias, shader] : Shader::getCache())
                for (const auto& info : shader->getShaderInfos())
                    addToFileSaveTimes(info.filename);

            for (const auto& [alias, material] : Material::getCache())
                addToFileSaveTimes(material->getFilePath());

            for (const auto& [alias, mesh] : Mesh::getCache())
                addToFileSaveTimes(mesh->getFilePath());

            // TEMPORARY: Uncomment when 1D and 3D textures are supported
            //for (const auto& [alias, texture1D] : Texture1D::getCache())
            //    addToFileSaveTimes(texture1D->getFilePath());

            for (const auto& [alias, texture2D] : Texture2D::getCache())
                addToFileSaveTimes(texture2D->getFilePath());

            //for (const auto& [alias, texture3D] : Texture3D::getCache())
            //    addToFileSaveTimes(texture3D->getFilePath());
        }
    }

    bool GuiLayer::updateAsset(const std::string& filepath, 
                               void (*loadAsset)(void*), 
                               void* userData)
    {
        // If the file has been saved to since the last load, reload it
        auto relPath     = std::fs::relative(filepath);
        auto currRelPath = std::fs::relative(m_fileSaveTimeIt->first);
        if (exists(relPath) && exists(currRelPath) &&
            std::fs::equivalent(currRelPath, relPath))
        {
            loadAsset(userData);
            return true;
        }
        return false;
    }

    inline void GuiLayer::drawAssetCache()
    {
        if (ImGui::Begin("AssetCache##DrawAssetCache"))
        {
            const int buf_size = 128;
            static char name[buf_size]{ 0 };
            static bool showDialogue = false;
            if (!showDialogue)
            {
                if (ImGui::Button("Add Material"))
                    showDialogue = true;
            }
            else
            {
                if (ImGui::InputText("Name##AssetCacheAddMatName", name, buf_size, ImGuiInputTextFlags_EnterReturnsTrue)
                    && name[0] != 0)
                {
                    bool valid = true;
                    for (const auto& [alias, material] : Material::getCache())
                        if (strcmp(name, alias.c_str()) == 0)
                        {
                            valid = false;
                            break;
                        }

                    if (valid)
                    {
                        Material::cache(Material::create(), name);

                        memset(name, 0, sizeof(name));
                        showDialogue = false;
                    }
                }
            }
            
            for (const auto& [alias, material] : Material::getCache())
            {
                if (ImGui::Selectable(alias.c_str(), m_currentSelection.material() == material, 
                                      ImGuiSelectableFlags_PressedOnClick))
                {
                    m_currentSelection = material;
                }
            }
        }
        ImGui::End();
    }

    void GuiLayer::drawSceneViewport()
    {
        // Only still here for easy navigation to the source code for learning imgui
        if constexpr (false)
            ImGui::ShowDemoWindow();

        // HACK: Make scene window the default on open
        {
            static bool _first = true;
            if (static bool _second = true; !_first && _second)
            {
                _second = false;
                ImGui::SetNextWindowFocus();
            }
            _first = false;
        }
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin(g_sceneWindowName, nullptr, ImGuiWindowFlags_NoScrollbar))
        {
            static ImVec2 lastSize = { 0, 0 };

            auto [x, y]   = ImGui::GetWindowSize();
            auto [cx, cy] = ImGui::GetCursorPos();

            if (lastSize.x != x || lastSize.y != y)
            {   
                EditorViewportResizedEvent vrevent;
                vrevent.width  = x;
                vrevent.height = y - cy;
                
                postEvent(vrevent);
            }
            
            y -= cy;
    
            ImGui::Image(
                (void*) m_editorViewportHandle,
                ImVec2(x, y),
                ImVec2(0, 1), ImVec2(1, 0)
            );

            using component::Camera;

            Camera* camera = nullptr;

            if (registry->valid(m_currentSelection.entity()))
                camera = registry->try_get<Camera>(m_currentSelection.entity());

            Ref<const FrameBuffer> buffer;

            if (camera) buffer = camera->forwardFrameBuffer();
            
            if (buffer && buffer->getColorHandle(0))
            {
                auto [posx, posy] = ImGui::GetItemRectMin();

                float camX = posx + x - 20;
                float camY = posy + y - 20;

                ImGui::SetNextWindowPos(ImVec2(camX, camY), 0, ImVec2(1, 1));
                ImVec2 cameraWindowSize = ImVec2(camera->aspect() * y / 4.0f, y / 4.0f + 20);
                ImGui::SetNextWindowSize(cameraWindowSize);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                if (ImGui::Begin("Camera Preview##ViewportCameraPreview", nullptr,
                                 ImGuiWindowFlags_NoScrollbar |
                                 ImGuiWindowFlags_NoScrollWithMouse |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoCollapse))
                {
                    ImGui::Image(
                        (void*) buffer->getColorHandle(0),
                        ImGui::GetWindowContentRegionMax() - ImVec2(0, 10),
                        ImVec2(0, 1), ImVec2(1, 0)
                    );
                }
                ImGui::PopStyleVar();

                ImGui::End();
            }
            
            ImGuizmo::SetDrawlist();
            auto [minX, minY]   = ImGui::GetItemRectMin();
            auto [sizeX, sizeY] = ImGui::GetItemRectSize();
            ImGuizmo::SetRect(minX, minY, sizeX, sizeY);
    
            if (ImGui::IsItemClicked(0) && !ImGuizmo::IsOver())
                m_currentSelection = Selectable(entt::entity(entt::null));

            if (ImGui::IsMouseClicked(1) || ImGui::IsMouseReleased(1))
            {
                EditorCameraMoveRequestEvent event;
                event.doMove = ImGui::IsItemClicked(1);
                postEvent(event);
            }
    
            drawTransformGizmo();
        }

        drawTransformGizmoSettings();

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GuiLayer::drawGameViewport()
    {        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        if (ImGui::Begin(g_gameWindowName, NULL, ImGuiWindowFlags_NoScrollbar))
        {
            auto [x, y] = ImGui::GetWindowSize();

            ImVec2 newPos = { 0, 0 };

            if (x / y < 16.0f / 9.0f)
            {
                newPos.y = (y - x * 9.0f / 16.0f) / 2.0f;
                y        = x * 9.0f / 16.0f;
            }
            else
            {
                newPos.x = (x - y * 16.0f / 9.0f) / 2.0f;
                x        = y * 16.0f / 9.0f;
            }

            ImGui::SetCursorPos(newPos);

            ImGui::Image(
                (void*) m_gameViewportHandle,
                ImVec2(x, y),
                ImVec2(0, 1), ImVec2(1, 0)
            );
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GuiLayer::drawTransformGizmoSettings()
    {
        if (ImGui::Begin(g_transformGizmosName, NULL, ImGuiWindowFlags_NoDecoration))
        {
            ImGui::SameLine(0, 10);
            
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
                    m_snap            = m_translateSnap;
                    break;
                case ImGuizmo::ROTATE:
                    ImGui::InputFloat("Snap##InputSnapRotate", &m_rotateSnap.x, 1.0f, 5.0f, "%.2f");
                    m_rotateSnap.z = m_rotateSnap.y = m_rotateSnap.x;
                    m_snap         = m_rotateSnap;
                    break;
                case ImGuizmo::SCALE:
                    ImGui::InputFloat("Snap##InputSnapScale", &m_scaleSnap.x, 0.1f, 0.5f, "%.2f");
                    m_scaleSnap.z = m_scaleSnap.y = m_scaleSnap.x;
                    m_snap        = m_scaleSnap;
                    break;
            }
        }
        ImGui::End();
    }

    void GuiLayer::drawTransformGizmo()
    {
        using component::Transform;
        
        if (registry->valid(m_currentSelection.entity()))
        {
            auto& model = registry->get<Transform>(m_currentSelection.entity());

            glm::mat4 gizmoMatrix = model.getMatrixGlobal();
            
            ImGuizmo::Manipulate(value_ptr(m_editorCamera->getViewMatrix()),
                                 value_ptr(m_editorCamera->getProjectionMatrix()),
                                 m_currentOp,
                                 m_currentMode,
                                 value_ptr(gizmoMatrix),
                                 nullptr,
                                 m_doSnap ? &m_snap.x : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::mat4 tempLocal;
                
                if (auto* p = registry->try_get<component::Parent>(m_currentSelection.entity());
                    p && registry->valid(p->parent))
                {   
                    auto& parentTransform = registry->get<Transform>(p->parent);

                    tempLocal = glm::inverse(parentTransform.getMatrixGlobal()) * gizmoMatrix;
                }
                else
                {
                    tempLocal = gizmoMatrix;    
                }
                
                glm::vec3 tComponents[3];
                ImGuizmo::DecomposeMatrixToComponents(value_ptr(tempLocal),
                                                      value_ptr(tComponents[0]),
                                                      value_ptr(tComponents[1]),
                                                      value_ptr(tComponents[2]));

                tComponents[2] = max(glm::vec3(0.01f), tComponents[2]);

                switch (m_currentOp)
                {
                    case ImGuizmo::TRANSLATE:
                    {
                        //UniqueRef<EditorTranslateEntityCommand>
                        model.setPosition(tComponents[0]);
                        //model.m_localPosition        = tComponents[0];
                        //model.m_isPositionOverridden = true;
                        break;
                    }
                    case ImGuizmo::ROTATE:
                    {
                        model.setRotation(glm::quat(radians(tComponents[1])));
                        //model.m_localRotation        = glm::quat(radians(tComponents[1]));
                        //model.m_isRotationOverridden = true;
                        break;
                    }
                    case ImGuizmo::SCALE:
                    {
                        model.setScale(tComponents[2]);
                        //model.m_localScale        = tComponents[2];
                        //model.m_isScaleOverridden = true;
                        break;
                    }
                }

                //model.m_isLocalDirty = true;
            }
        }
    }

    void GuiLayer::drawPlayPauseButtons()
    {
        if (ImGui::Begin(g_playPauseWindowName), NULL, ImGuiWindowFlags_NoDecoration)
        {
            if (m_editorOverrideUpdate)
            {
                if (ImGui::ArrowButton("##EditorPlayButton", ImGuiDir_Right))
                {
                    ImGui::SetWindowFocus(g_gameWindowName);
                    
                    m_editorOverrideUpdate = false;
                    m_gameUpdate           = true;
                    m_registryRestore      = Scene::current()->getRegistry()->clone();
                    m_currentSelection     = entt::entity(entt::null);
                }
            } else
            {
                if (ImGui::ArrowButton("##EditorBackButton", ImGuiDir_Left))
                {
                    ImGui::SetWindowFocus(g_sceneWindowName);
                    
                    m_editorOverrideUpdate = true;
                    m_gameUpdate           = false;

                    // HACK: Send as event
                    // TODO: Store backup registry in scene?
                    *Scene::current()->m_registry = m_registryRestore.clone();

                    postEvent(PhysicsResetWorldEvent{});

                    m_currentSelection = entt::entity(entt::null);
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

        auto & style                                = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text]                 = TEXT(0.78f);
        style.Colors[ImGuiCol_TextDisabled]         = TEXT(0.28f);
        style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_PopupBg]              = BG(0.9f);
        style.Colors[ImGuiCol_Border]               = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
        style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg]              = BG(1.00f);
        style.Colors[ImGuiCol_FrameBgHovered]       = MED(0.78f);
        style.Colors[ImGuiCol_FrameBgActive]        = MED(1.00f);
        style.Colors[ImGuiCol_TitleBg]              = LOW(1.00f);
        style.Colors[ImGuiCol_TitleBgActive]        = HI(1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed]     = BG(0.75f);
        style.Colors[ImGuiCol_MenuBarBg]            = BG(0.47f);
        style.Colors[ImGuiCol_ScrollbarBg]          = BG(1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
        style.Colors[ImGuiCol_ScrollbarGrabActive]  = MED(1.00f);
        style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        style.Colors[ImGuiCol_Button]               = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        style.Colors[ImGuiCol_ButtonHovered]        = MED(0.86f);
        style.Colors[ImGuiCol_ButtonActive]         = MED(1.00f);
        style.Colors[ImGuiCol_Header]               = MED(0.76f);
        style.Colors[ImGuiCol_HeaderHovered]        = MED(0.86f);
        style.Colors[ImGuiCol_HeaderActive]         = HI(1.00f);
        style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
        style.Colors[ImGuiCol_ResizeGripHovered]    = MED(0.78f);
        style.Colors[ImGuiCol_ResizeGripActive]     = MED(1.00f);
        style.Colors[ImGuiCol_PlotLines]            = TEXT(0.63f);
        style.Colors[ImGuiCol_PlotLinesHovered]     = MED(1.00f);
        style.Colors[ImGuiCol_PlotHistogram]        = TEXT(0.63f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
        style.Colors[ImGuiCol_TextSelectedBg]       = MED(0.43f);
        style.Colors[ImGuiCol_Tab]                  = LOW(0.43f);
        style.Colors[ImGuiCol_TabActive]            = MED(1.00f);
        style.Colors[ImGuiCol_TabHovered]           = HI(1.00f);
        style.Colors[ImGuiCol_TabUnfocused]         = LOW(0.88f);
        style.Colors[ImGuiCol_TabUnfocusedActive]   = MED(0.73f);

        //style.Colors[ImGuiCol_Tab] = ImLerp(style.Colors[ImGuiCol_Header], style.Colors[ImGuiCol_TitleBgActive], 0.80f);
        //style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered];
        //style.Colors[ImGuiCol_TabActive] = ImLerp(style.Colors[ImGuiCol_HeaderActive], style.Colors[ImGuiCol_TitleBgActive], 0.60f);
        //style.Colors[ImGuiCol_TabUnfocused] = ImLerp(style.Colors[ImGuiCol_Tab], style.Colors[ImGuiCol_TitleBg], 0.80f);
        //style.Colors[ImGuiCol_TabUnfocusedActive] = ImLerp(style.Colors[ImGuiCol_TabActive], style.Colors[ImGuiCol_TitleBg], 0.40f);
        
        // [...]
        style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.73f);

        style.WindowPadding     = ImVec2(6, 4);
        style.WindowRounding    = 0.0f;
        style.FramePadding      = ImVec2(5, 2);
        style.FrameRounding     = 3.0f;
        style.ItemSpacing       = ImVec2(7, 1);
        style.ItemInnerSpacing  = ImVec2(1, 1);
        style.TouchExtraPadding = ImVec2(0, 0);
        style.IndentSpacing     = 15.0f;
        style.ScrollbarSize     = 12.0f;
        style.ScrollbarRounding = 16.0f;
        style.GrabMinSize       = 20.0f;
        style.GrabRounding      = 2.0f;

        style.WindowTitleAlign.x = 0.50f;

        style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
        style.FrameBorderSize         = 0.0f;
        style.WindowBorderSize        = 1.0f;
    }
}

static bool isTexture(const char* ext)
{
    return strcmp(ext, ".jpg") == 0 ||
           strcmp(ext, ".png") == 0 ||
           strcmp(ext, ".bmp") == 0;
}

static bool isMesh(const char* ext)
{
    return strcmp(ext, ".obj") == 0;
}

static bool isShader(const char* ext)
{
    return strcmp(ext, ".vert") == 0 ||
           strcmp(ext, ".tesc") == 0 ||
           strcmp(ext, ".tese") == 0 ||
           strcmp(ext, ".geom") == 0 ||
           strcmp(ext, ".frag") == 0;
}

static bool isMaterial(const char* ext)
{
    return strcmp(ext, ".oylmat") == 0;
}

static bool isScene(const char* ext)
{
    return strcmp(ext, ".oylscene") == 0;
}

static bool isPrefab(const char* ext)
{
    return strcmp(ext, ".oylpre") == 0;
}
