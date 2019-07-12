#pragma once

#include "Oyl3D/System/Layer.h"

namespace oyl {

class ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	virtual ~ImGuiLayer();

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onUpdate() override;
	virtual void onEvent(Event& e) override;

	bool onMouseMove(MouseMoveEvent& e);
	bool onMousePress(MousePressEvent& e);
	bool onMouseRelease(MouseReleaseEvent& e);
	bool onMouseScroll(MouseScrollEvent& e);
	bool onKeyPress(KeyPressEvent& e);
	bool onKeyRelease(KeyReleaseEvent& e);
	bool onKeyType(KeyTypeEvent& e);
private:
	float m_time;
};

}