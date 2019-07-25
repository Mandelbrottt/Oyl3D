#pragma once

#include "Oyl3D/Graphics/Buffer.h"
#include "Oyl3D/Graphics/Mesh.h"

namespace oyl {

class RendererAPI {
public:
	enum class API {
		None = 0,
		OpenGL
	};
public:
	virtual void setClearColor(float r, float g, float b, float a) = 0;
	virtual void clear() = 0;

	virtual void drawIndexed(const std::shared_ptr<VertexArray>& vao) = 0;
	virtual void drawMesh(const std::shared_ptr<Mesh>& mesh) = 0;

	inline static API getAPI() { return s_API; }
private:
	static API s_API;
};

}