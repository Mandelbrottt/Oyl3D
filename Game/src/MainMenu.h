#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"
#include "ScrollingTextureLayer.h"

class MainMenuLayer : public oyl::Layer
{
public:
    OYL_CTOR(MainMenuLayer, Layer)

	const float CHANGE_MENU_OPTION_DELAY = 0.15f;
	float changeMenuOptionCountdown      = 0.0f;

	float delayBeforeAcceptingInputCountdown;

    virtual void onEnter() override;

    virtual void onUpdate() override;

	virtual bool onEvent(const oyl::Event& event) override;

    virtual void onGuiRender() override;
};

class MainMenuScene : public oyl::Scene
{
public:
	OYL_CTOR(MainMenuScene, oyl::Scene)

	virtual void onEnter() override
	{
		pushLayer(MainMenuLayer::create());
		pushLayer(ScrollingTextureLayer::create());
	}
};