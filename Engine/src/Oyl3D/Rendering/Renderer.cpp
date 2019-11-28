#include "oylpch.h"
#include "Renderer.h"

#include "Graphics/Shader.h"
#include "Graphics/Material.h"

namespace oyl
{
    //Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

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

        // TEMPORARY:
        shader->setUniformMat4("u_model", transform);
        
        RenderCommand::drawIndexed(vao);
    }

    // Assumes that there is only one texture, will be changed when material file system is implemented
    void Renderer::submit(const Ref<Mesh>& mesh, const Ref<Material>& material, glm::mat4 transform)
    {
        mesh->bind();

        material->bind();
        
        // TEMPORARY:
        material->getShader()->setUniformMat4("u_model", transform);

        RenderCommand::drawMesh(mesh);
    }
}
