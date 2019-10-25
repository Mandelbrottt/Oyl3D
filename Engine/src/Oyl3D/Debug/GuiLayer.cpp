#include "oylpch.h"
#include "GuiLayer.h"

#include "App/Application.h"

#include "ECS/Component.h"
#include "ECS/Registry.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

static const char* entityNodeFmt = "%s\t\t\t\t\t\t\t\t\t\t";

namespace oyl
{
    void GuiLayer::onEnter()
    {
        setupGuiLibrary();

        addToEventMask(TypeViewportHandleChanged);
        addToEventMask(TypeEditorEntitySelected);
        addToEventMask(TypeMousePressed);
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
                                         ImGuiWindowFlags_NoNavFocus);

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        bool p_open = true;
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);
        ImGui::PopStyleVar(3);

        OYL_ASSERT(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable, "Docking should always be enabled!");

        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
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
        drawViewport();

        if (ImGui::Begin("Scene Hierarchy"))
        {
            registry->each(
                [this](auto entity)
                {
                    auto& so = registry->get_or_assign<Component::SceneObject>(entity);
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
                    bool clicked = ImGui::IsItemClicked(0);
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

        if (ImGui::Begin("Entity Properties"))
        {
            if (m_currentEntity != u32(-1))
            {
                if (ImGui::CollapsingHeader("Transform"))
                {
                    ImGui::Indent();

                    auto& transform = registry->get_or_assign<Component::Transform>(Entity(m_currentEntity));

                    ImGui::PushItemWidth(15);

                    ImGui::DragFloat("X", &transform.position[0], 0.2f, 0, 0, "");
                    ImGui::SameLine();
                    ImGui::DragFloat("Y", &transform.position[1], 0.2, 0, 0, "");
                    ImGui::SameLine();
                    ImGui::DragFloat("Z", &transform.position[2], 0.2f, 0, 0, "");

                    ImGui::PopItemWidth();

                    ImGui::InputFloat3("Position", &transform.position[0]);
                    ImGui::InputFloat3("Rotation", &transform.rotation[0]);
                    ImGui::InputFloat3("Scale", &transform.scale[0]);

                    ImGui::Unindent();
                }

                if (ImGui::BeginCombo("COMBO", "COMBO"))
                {

                    ImGui::Image(
                        (void*)m_viewportHandle,
                        ImVec2(100, 100),
                        ImVec2(0, 1), ImVec2(1, 0));

                    ImGui::EndCombo();
                }
            }
        }
        ImGui::End();
    }

    bool GuiLayer::onEvent(Ref<Event> event)
    {
        switch (event->type)
        {
            case TypeViewportHandleChanged:
            {
                auto e = (ViewportHandleChangedEvent) *event;
                m_viewportHandle = e.handle;
            }
            case TypeEditorEntitySelected:
            {
                auto e = (EditorEntitySelectedEvent) *event;
                m_currentEntity = u32(e.entity);
            }
            case TypeMousePressed:
            {
                //auto e = (MousePressedEvent) *event;
                //if (e.button == Mouse_Left)
                //{
                //}
            }
        }
        return false;
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

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        Application& app = Application::get();
        GLFWwindow*  window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 420");
    }

    void GuiLayer::drawViewport()
    {
        // Only still here for easy navigation to the source code for learning imgui
        if constexpr (false)
            ImGui::ShowDemoWindow();

        bool neededBool = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport", &neededBool, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

        auto [x, y] = ImGui::GetWindowSize();
        y -= ImGui::GetCursorPosY();

        float camX = ImGui::GetCursorScreenPos().x + x - 20;
        float camY = ImGui::GetCursorScreenPos().y + y - 20;
        
        ImGui::SetNextWindowPos(ImVec2(camX, camY), 0, ImVec2(1, 1));
        ImVec2 cameraWindowSize = ImVec2(x / 4, y / 4);
        ImGui::SetNextWindowSize(cameraWindowSize);
        
        ImGui::Begin("Camera Viewport", &neededBool, 
                     ImGuiWindowFlags_NoScrollbar | 
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize);
        ImGui::PopStyleVar();

        ImGui::Image(
            (void*) m_viewportHandle,
            cameraWindowSize,
            ImVec2(0, 1), ImVec2(1, 0)
        );

        ImGui::End();
        
        ViewportResizedEvent vrevent;
        vrevent.id = 0;
        vrevent.width = x;
        vrevent.height = y;

        postEvent(Event::create(vrevent));

        ImGui::Image(
            (void*) m_viewportHandle,
            ImVec2(x, y),
            ImVec2(0, 1), ImVec2(1, 0)
        );

        ImGui::End();
    }
}
