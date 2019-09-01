#include <Oyl3D.h>

class MainLayer : public oyl::Layer {
public:
	OYL_CREATE_FUNC(MainLayer)

	MainLayer() : Layer("Main") {}

	virtual void onAttach() override {
		m_mesh = oyl::Mesh::create("res/capsule.obj");
		m_mesh->loadTexture("res/capsule0.jpg");

		m_meshShader = oyl::Shader::create({
			{ oyl::VertexShader, "../Engine/res/meshShader.vert" },
			{ oyl::FragmentShader, "../Engine/res/meshShader.frag" },
											   });
	}

	virtual void onDetach() override {}

	virtual void onUpdate(oyl::Timestep dt) override {
		m_timeSince += dt;

		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, glm::vec3(0, 0, -5));
		transform = glm::rotate(transform, m_timeSince, glm::vec3(0.0f, 1.0f, 0.5f));

		oyl::Renderer::submit(m_meshShader, m_mesh, transform);
	}

	virtual void onImGuiRender() override {

	}

	virtual void onEvent(oyl::Event& event) override {
		oyl::EventListener dispatcher(event);
		dispatcher.dispatch<oyl::KeyReleaseEvent>([](oyl::KeyReleaseEvent e)->bool {
			oyl::Window& window = oyl::Application::get().getWindow();
			switch (e.getKeyCode()) {
			case oyl::Key_F11:
				if (window.getFullscreenType() == oyl::Windowed) {
					window.setFullscreenType(oyl::Fullscreen);
				} else {
					window.setFullscreenType(oyl::Windowed);
				}
				break;
			case oyl::Key_F7:
				window.setVsync(!window.isVsync());
				break;
			}
			return false;
												  });
	}
private:
	oyl::Ref<oyl::Mesh> m_mesh;
	oyl::Ref<oyl::Shader> m_meshShader;

	float m_timeSince = 0.0f;
};

class MainScene : public oyl::Scene {
public:
	OYL_CREATE_FUNC(MainScene)

	MainScene() : oyl::Scene("MainScene") {}
	~MainScene() {}

	virtual void onEnter() {
		m_mainLayer = MainLayer::create();
		pushLayer(m_mainLayer);
	}

	virtual void onExit() {
		popLayer(m_mainLayer);
	}
private:
	oyl::Ref<MainLayer> m_mainLayer;
};

class Game : public oyl::Application {
public:
	Game() {
		pushScene(MainScene::create());
	}

	virtual void onExit() {}
public:
	OYL_CREATE_FUNC(MainScene)
};

oyl::Application* oyl::createApplication() {
	return new Game();
}