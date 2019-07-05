#include <Oyl3D.h>

class Game : public oyl::Application {
public:
	Game() {

	}

	~Game() {

	}
};

oyl::Application* oyl::createApplication() {
	return new Game();
}