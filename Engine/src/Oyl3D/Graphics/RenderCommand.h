#pragma once

#include "RendererAPI.h"

namespace oyl {

class RenderCommand {
public:
	inline static void setClearColor(const glm::vec4& color) {
		s_rendererAPI->setClearColor(color.r, color.g, color.b, color.a);
	}

	inline static void setClearColor(float r, float g, float b, float a) {
		s_rendererAPI->setClearColor(r, g, b, a);
	}

	inline static void clear() {
		s_rendererAPI->clear();
	}

	inline static void drawIndexed(const std::shared_ptr<VertexArray>& vao) {
		s_rendererAPI->drawIndexed(vao);
	}
private:
	static RendererAPI* s_rendererAPI;
};

}