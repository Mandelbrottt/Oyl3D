#include <Oyl3D.h>

class Game : public oyl::Application {
public:
	Game() {
		pushOverlay(new oyl::ImGuiLayer());
	}

	~Game() {

	}
};

oyl::Application* oyl::createApplication() {
	return new Game();
}