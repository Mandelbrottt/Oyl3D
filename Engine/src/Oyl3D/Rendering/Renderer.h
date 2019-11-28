#pragma once

#include "Oyl3D/Rendering/RenderCommand.h"

namespace oyl
{
    class Shader;
    class Material;
    class TextureCubeMap;

    class Renderer
    {
    public:
        // TODO: make a seperate beginGUI for ortho, beginScene for perspective
        static void beginScene();
        static void endScene();

        static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vao, glm::mat4 transform = glm::mat4(1.0f));
        static void submit(const Ref<Material>& material, const Ref<VertexArray>& vao, u32 count, glm::mat4 transform = glm::mat4(1.0f));
        static void submit(const Ref<Mesh>& mesh, const Ref<Material>& material, glm::mat4 transform = glm::mat4(1.0f));
        static void submit(const Ref<Mesh>& mesh, const Ref<Shader>& shader, const Ref<TextureCubeMap>& cubemap);

        inline static OylEnum getAPI() { return RendererAPI::getAPI(); }
    };
}
