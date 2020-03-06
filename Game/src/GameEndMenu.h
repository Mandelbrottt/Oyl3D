#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

class GameEndLayer : public oyl::Layer
{
public:
	OYL_CTOR(GameEndLayer, Layer)

	MenuOption selectedMenuItemType;

	const float CHANGE_MENU_OPTION_DELAY = 0.1f;
	float changeMenuOptionCountdown      = 0.0f;

	virtual void onEnter() override;

	virtual void onUpdate() override;

	virtual bool onEvent(const oyl::Event& event) override;

	virtual void onGuiRender() override;
};

class GameEndScene : public oyl::Scene
{
public:
	OYL_CTOR(GameEndScene, oyl::Scene)

	virtual void onEnter() override
	{
		pushLayer(GameEndLayer::create());
	}
};