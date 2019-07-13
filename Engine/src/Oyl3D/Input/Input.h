#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl {

class Input {
public:
	inline static bool isKeyPressed(int keyCode) { return s_instance->isKeyPressedImpl(keyCode);  }
	inline static bool isMousePressed(int button = 0) { return s_instance->isMousePressedImpl(button); }

	inline static glm::vec2 getMousePos() { return s_instance->getMousePosImpl(); }
	inline static float getMouseX() { return s_instance->getMouseXImpl(); }
	inline static float getMouseY() { return s_instance->getMouseYImpl(); }
protected:
	virtual bool isKeyPressedImpl(int keyCode) = 0;
	virtual bool isMousePressedImpl(int button) = 0;

	virtual glm::vec2 getMousePosImpl() = 0;
	virtual float getMouseXImpl() = 0;
	virtual float getMouseYImpl() = 0;
private:
	static Input* s_instance;
};

}