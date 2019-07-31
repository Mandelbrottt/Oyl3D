#include "oylpch.h"
#include "OpenGLRendererAPI.h"

#include "GLCommon.h"

#include <glad/glad.h>

namespace oyl {

void OpenGLRendererAPI::setClearColor(float r, float g, float b, float a) {
	GLCall(glClearColor(r, g, b, a));
}

void OpenGLRendererAPI::clear() {
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void OpenGLRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vao) {
	GLCall(glDrawElements(GL_TRIANGLES, vao->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr));
}

void OpenGLRendererAPI::drawMesh(const std::shared_ptr<Mesh>& mesh) {
	GLCall(glDrawArrays(GL_TRIANGLES, 0, mesh->getNumVertices()));
}

}

