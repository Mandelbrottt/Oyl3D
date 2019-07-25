#include "oylpch.h"
#include "Renderer.h"

namespace oyl {

Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

void Renderer::beginScene(const OrthographicCamera& camera) {
	s_sceneData->orthoVPMatrix = camera.getViewProjectionMatrix();
}

void Renderer::beginScene(const PerspectiveCamera& camera) {
	s_sceneData->perspectiveViewMatrix = camera.getViewMatrix();
	s_sceneData->perspectiveProjectionMatrix = camera.getProjectionMatrix();
}


void Renderer::endScene() {

}

void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, glm::mat4 transform) {
	shader->bind();
	shader->setUniformMat4("u_viewProjection", s_sceneData->orthoVPMatrix);
	shader->setUniformMat4("u_model", transform);
	vao->bind();
	RenderCommand::drawIndexed(vao);
}

void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Mesh>& mesh, glm::mat4 transform) {
	shader->bind();
	shader->setUniformMat4("u_model", transform);
	shader->setUniformMat4("u_view", s_sceneData->perspectiveViewMatrix);
	shader->setUniformMat4("u_projection", s_sceneData->perspectiveProjectionMatrix);
	mesh->bind();
	RenderCommand::drawMesh(mesh);
}


}

