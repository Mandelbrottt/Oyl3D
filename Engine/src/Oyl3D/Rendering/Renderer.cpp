#include "oylpch.h"
#include "Renderer.h"

#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace oyl
{
    void Renderer::beginScene() { }

    void Renderer::endScene() { }

    // TEMPORARY:
    void Renderer::submit(const Ref<Material>& material, const Ref<VertexArray>& vao, u32 count, glm::mat4 transform)
    {
        vao->bind();

        material->bind();
        
        // TEMPORARY:
        material->getShader()->setUniformMat4("u_model", transform);
        
        RenderCommand::drawArrays(vao, count);
    }
    
    void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vao, glm::mat4 transform)
    {
        vao->bind();
        
        shader->bind();

        shader->setUniformMat4("u_model", transform);
        
        RenderCommand::drawIndexed(vao);
    }

    void Renderer::submit(const Ref<Mesh>& mesh, const Ref<Material>& material, glm::mat4 transform)
    {
        mesh->bind();

        material->bind();
        
        // TEMPORARY:
        material->getShader()->setUniformMat4("u_model", transform);

        RenderCommand::drawMesh(mesh);
    }

    void Renderer::submit(const Ref<Mesh>& mesh, const Ref<Shader>& shader, const Ref<TextureCubeMap>& cubemap)
    {
        mesh->bind();

        shader->bind();

        cubemap->bind(0);
        shader->setUniform1i("u_skybox", 0);

        RenderCommand::drawMesh(mesh);
    }
}
