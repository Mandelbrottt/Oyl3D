#pragma once

#include "RenderCommand.h"

#include "Graphics/Camera.h"
#include "Graphics/Shader.h"

namespace oyl {

class Renderer {
public:
	// TODO: make a seperate beginGUI for ortho, beginScene for perspective
	static void beginScene(const OrthographicCamera& camera);
	static void endScene();

	static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao);

	inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
private:
	struct SceneData {
		glm::mat4 viewProjectionMatrix;
	};

	static SceneData* s_sceneData;
};

}