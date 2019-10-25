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

        if (ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse))
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

        if (ImGui::Begin("Entity Properties", nullptr))
        {
            if (m_currentEntity != u32(-1))
            {
                if (ImGui::CollapsingHeader("Transform"))
                {
                    ImGui::Indent(5);

                    auto& transform = registry->get_or_assign<Component::Transform>(Entity(m_currentEntity));

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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

        auto [x, y] = ImGui::GetWindowSize();
        y -= ImGui::GetCursorPosY();

        float camX = ImGui::GetCursorScreenPos().x + x - 20;
        float camY = ImGui::GetCursorScreenPos().y + y - 20;
        
        ImGui::SetNextWindowPos(ImVec2(camX, camY), 0, ImVec2(1, 1));
        ImVec2 cameraWindowSize = ImVec2(x / 4, y / 4);
        ImGui::SetNextWindowSize(cameraWindowSize);
        
        if (ImGui::Begin("Camera Viewport", nullptr,
                         ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoResize))
        {

            ImGui::Image(
                (void*) m_viewportHandle,
                cameraWindowSize,
                ImVec2(0, 1), ImVec2(1, 0)
            );
        }
        else
        {
            auto [newX, newY] = ImGui::GetWindowPos();
            newY -= cameraWindowSize.y;
            ImGui::SetWindowPos("Camera Viewport", ImVec2(newX, newY));
        }
        ImGui::PopStyleVar();
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
