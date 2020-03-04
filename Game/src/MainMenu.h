#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

class MainMenuLayer : public oyl::Layer
{
public:
    OYL_CTOR(MainMenuLayer, Layer)

    virtual void onEnter() override;

    virtual void onUpdate() override;

    virtual void onGuiRender() override;
};

class MainMenuScene : public oyl::Scene
{
public:
	OYL_CTOR(MainMenuScene, oyl::Scene)

	virtual void onEnter() override
	{
		pushLayer(MainMenuLayer::create());
	}
};