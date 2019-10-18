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
        using Component::MeshRenderer;

        Ref<Registry> reg = Scene::current()->getRegistry();
        
        // We sort our mesh renderers based on material properties
        // This will group all of our meshes based on shader first, then material second
        reg->sort<MeshRenderer>([](const MeshRenderer& lhs, const MeshRenderer& rhs)
        {
            if (rhs.material == nullptr || rhs.mesh == nullptr)
                return false;
            else if (lhs.material == nullptr || lhs.mesh == nullptr)
                return true;
            else if (lhs.material->getShader() != rhs.material->getShader())
                return lhs.material->getShader() < rhs.material->getShader();
            else
                return lhs.material < rhs.material;
        });

        Ref<Shader>   boundShader;
        Ref<Material> boundMaterial;

        auto view = reg->view<MeshRenderer>();
        for (const auto& entity : view)
        {
            MeshRenderer& mr = reg->get<MeshRenderer>(entity);

            if (mr.mesh == nullptr || mr.material == nullptr)
                continue;

            if (mr.material->getShader() != boundShader)
            {
                boundShader = mr.material->getShader();
                boundShader->bind();
            }

            if (mr.material != boundMaterial)
            {
                boundMaterial = mr.material;
                boundMaterial->applyUniforms();
            }

            const glm::mat4& transform = reg->get_or_assign<Transform>(entity).getMatrix();

            Renderer::submit(mr.mesh, mr.material, transform);
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
