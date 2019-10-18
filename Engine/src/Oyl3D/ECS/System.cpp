#include "oylpch.h"

#include "System.h"

#include "ECS/Component.h"

#include "Events/EventListener.h"

#include "Rendering/Renderer.h"

#include "Scenes/Scene.h"

namespace oyl::ECS
{
    // vvv Generic System vvv //
    void System::onEnter()
    {
    }

    void System::onExit()
    {
    }

    void System::onUpdate(Timestep dt)
    {
    }

    void System::onGuiRender()
    {
    }

    // ^^^ Generic System ^^^ //

    // vvv Render System vvv //

    void RenderSystem::onEnter()
    {
    }

    void RenderSystem::onExit()
    {
    }

    void RenderSystem::onUpdate(Timestep dt)
    {
        using Component::Transform;
        using Component::Model;

        Ref<Registry> reg = Scene::current()->getRegistry();
        
        auto view = reg->view<Transform, Model>();
        for (auto& entity : view)
        {
            const Ref<Mesh>& mesh = reg->get<Model>(entity).mesh;
            const glm::mat4& transform = reg->get<Transform>(entity).getMatrix();
            Renderer::submit(mesh, transform);
        }
    }

    void RenderSystem::onGuiRender()
    {
    }

    bool RenderSystem::onEvent(Ref<Event> event)
    {
        return false;
    }

    // ^^^ Render System ^^^ //
}
