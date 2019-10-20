#pragma once

#include "Oyl3D/Rendering/RenderCommand.h"

#include "Oyl3D/Graphics/Camera.h"
#include "Oyl3D/Graphics/Mesh.h"
#include "Oyl3D/Graphics/Shader.h"

namespace oyl
{
    class Renderer
    {
    public:
        // TODO: make a seperate beginGUI for ortho, beginScene for perspective
        static void beginScene();
        static void endScene();

        static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vao, glm::mat4 transform = glm::mat4(1.0f));
        static void submit(const Ref<Mesh>& mesh, const Ref<Material>& material, glm::mat4 transform = glm::mat4(1.0f));

        inline static OylEnum getAPI() { return RendererAPI::getAPI(); }
    };
}
