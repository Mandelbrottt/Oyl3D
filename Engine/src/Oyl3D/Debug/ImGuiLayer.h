#pragma once

#include "Oyl3D/System/Layer.h"

namespace oyl {

class ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	virtual ~ImGuiLayer();

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onImGuiRender() override;

	void begin();
	void end();
private:
	float m_time;
};

}