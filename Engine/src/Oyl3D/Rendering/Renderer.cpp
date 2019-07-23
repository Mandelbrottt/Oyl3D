#include "oylpch.h"
#include "Renderer.h"

namespace oyl {

Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

void Renderer::beginScene(const OrthographicCamera& camera) {
	s_sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::endScene() {

}

void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao) {
	shader->bind();
	shader->setUniformMat4("u_viewProjection", s_sceneData->viewProjectionMatrix);
	vao->bind();
	RenderCommand::drawIndexed(vao);
}

}

