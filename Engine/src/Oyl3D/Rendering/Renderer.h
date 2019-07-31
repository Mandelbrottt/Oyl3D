#pragma once

#include "Oyl3D/Rendering/RenderCommand.h"

#include "Oyl3D/Graphics/Camera.h"
#include "Oyl3D/Graphics/Mesh.h"
#include "Oyl3D/Graphics/Shader.h"

namespace oyl {

class Renderer {
public:
	// TODO: make a seperate beginGUI for ortho, beginScene for perspective
	static void beginScene(const OrthographicCamera& camera);
	// TEMPORARY:
	static void beginScene(const PerspectiveCamera& camera);
	static void endScene();

	static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, glm::mat4 transform = glm::mat4(1.0f));
	static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Mesh>& mesh, glm::mat4 transform = glm::mat4(1.0f));

	inline static OylEnum getAPI() { return RendererAPI::getAPI(); }
private:
	struct SceneData {
		glm::mat4 orthoVPMatrix;
		glm::mat4 perspectiveViewMatrix;
		glm::mat4 perspectiveProjectionMatrix;
	};

	static SceneData* s_sceneData;
};

}