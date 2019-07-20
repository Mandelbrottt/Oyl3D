#pragma once

#include "RenderCommand.h"

namespace oyl {

class Renderer {
public:
	static void beginScene();
	static void endScene();

	static void submit(const std::shared_ptr<VertexArray>& vao);

	inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
};

}