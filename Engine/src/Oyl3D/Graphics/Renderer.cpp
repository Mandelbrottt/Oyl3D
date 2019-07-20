#include "oylpch.h"
#include "Renderer.h"

namespace oyl {

void Renderer::beginScene() {

}

void Renderer::endScene() {

}

void Renderer::submit(const std::shared_ptr<VertexArray>& vao) {
	vao->bind();
	RenderCommand::drawIndexed(vao);
}

}

