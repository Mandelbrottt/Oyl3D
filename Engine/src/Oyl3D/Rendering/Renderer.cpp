#include "oylpch.h"
#include "Renderer.h"

namespace oyl
{
    Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

    void Renderer::beginScene(const OrthographicCamera& camera)
    {
        s_sceneData->orthoVPMatrix = camera.getViewProjectionMatrix();
    }

    void Renderer::beginScene(const PerspectiveCamera& camera)
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
    void Renderer::submit(const Ref<Mesh>& mesh, glm::mat4 transform)
    {
        mesh->bind();
        Ref<Material> mat = mesh->getMaterial();

        mat->bind();
        mat->applyUniforms();

        // TEMPORARY:
        mat->setUniformMat4("u_view", s_sceneData->perspectiveViewMatrix);
        mat->setUniformMat4("u_projection", s_sceneData->perspectiveProjectionMatrix);

        RenderCommand::drawMesh(mesh);
    }
}
