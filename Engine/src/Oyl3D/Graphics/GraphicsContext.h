#pragma once

namespace oyl {

class GraphicsContext {
public:
	virtual void init() = 0;
	virtual void swapBuffers() = 0;
	virtual void updateViewport() = 0;
};

}