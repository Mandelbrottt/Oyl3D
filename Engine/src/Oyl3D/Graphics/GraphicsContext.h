#pragma once

namespace oyl {

class GraphicsContext {
public:
	virtual void init() = 0;
	virtual void swapBuffers() = 0;
	virtual void updateViewport(int width, int height) = 0;

	static GraphicsContext* create(void* window);
};

}