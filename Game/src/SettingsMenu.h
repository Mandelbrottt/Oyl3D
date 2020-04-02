#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

class SettingsMenuLayer : public oyl::Layer
{
public:
	OYL_CTOR(SettingsMenuLayer, Layer)

	const float SLIDER_CHANGE_DELAY = 0.15f;
	float sliderChangeDelayCountdown = 0.0f;

	virtual void onEnter() override;

	virtual void onUpdate() override;

	virtual bool onEvent(const oyl::Event& event) override;

	virtual void onGuiRender() override;
};

class SettingsMenuScene : public oyl::Scene
{
public:
	OYL_CTOR(SettingsMenuScene, oyl::Scene)

	virtual void onEnter() override
	{
		pushLayer(SettingsMenuLayer::create());
	}
};