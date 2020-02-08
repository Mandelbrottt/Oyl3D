#pragma once
#include <Oyl3D.h>

using namespace oyl;
using namespace component;
class ScrollingTextureLayer : public Layer
{
	OYL_CTOR(ScrollingTextureLayer, oyl::Layer)

public:
	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	void onGuiRender() override;
	bool onEvent(const oyl::Event& event) override;;
};