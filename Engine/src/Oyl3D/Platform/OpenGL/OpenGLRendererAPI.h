#pragma once

#include "Oyl3D/Rendering/RendererAPI.h"

namespace oyl {

class OpenGLRendererAPI : public RendererAPI {
	virtual void setClearColor(float r, float g, float b, float a) override;
	virtual void clear() override;

	virtual void drawIndexed(const ref<VertexArray>& vao) override;
	virtual void drawMesh(const ref<Mesh>& mesh) override;
};

}