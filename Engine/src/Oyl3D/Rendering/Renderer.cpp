#include "oylpch.h"
#include "Renderer.h"

namespace oyl
{
    Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

    void Renderer::beginScene(const Camera& camera)
    {
        s_sceneData->perspectiveViewMatrix       = camera.getViewMatrix();
        s_sceneData->perspectiveProjectionMatrix = camera.getProjectionMatrix();
    }

    void Renderer::endScene()
    {
    }

    void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vao, glm::mat4 transform)
    {
        shader->bind();
        shader->setUniformMat4("u_viewProjection", s_sceneData->orthoVPMatrix);
        shader->setUniformMat4("u_model", transform);
        vao->bind();
        RenderCommand::drawIndexed(vao);
    }

    // Assumes that there is only one texture, will be changed when material file system is implemented
    void Renderer::submit(const Ref<Mesh>& mesh, const Ref<Material>& material, glm::mat4 transform)
    {
        mesh->bind();

        material->bind();
        
        // TEMPORARY:
        material->setUniformMat4("u_model", transform);
        material->setUniformMat4("u_view", s_sceneData->perspectiveViewMatrix);
        material->setUniformMat4("u_projection", s_sceneData->perspectiveProjectionMatrix);

        material->applyUniforms();

        RenderCommand::drawMesh(mesh);
    }
}
