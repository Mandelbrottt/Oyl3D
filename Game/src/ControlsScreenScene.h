#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

class ControlsScreenLayer : public oyl::Layer
{
public:
	OYL_CTOR(ControlsScreenLayer, Layer)

	virtual void onEnter() override;

	virtual void onUpdate() override;

	virtual bool onEvent(const oyl::Event& event) override;

	virtual void onGuiRender() override;
};

class ControlsScreenScene : public oyl::Scene
{
public:
	OYL_CTOR(ControlsScreenScene, oyl::Scene)

	virtual void onEnter() override
	{
		pushLayer(ControlsScreenLayer::create());
	}
};