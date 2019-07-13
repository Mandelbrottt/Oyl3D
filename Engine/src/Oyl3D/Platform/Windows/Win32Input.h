#pragma once

#include "Oyl3D/Input/Input.h"

namespace oyl {

class Win32Input : public Input {
	
protected:
	virtual bool isKeyPressedImpl(int keyCode) override;
	virtual bool isMousePressedImpl(int button) override;

	virtual glm::vec2 getMousePosImpl() override;
	virtual float getMouseXImpl() override;
	virtual float getMouseYImpl() override;

};

}