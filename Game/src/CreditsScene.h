#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

class CreditsLayer : public oyl::Layer
{
public:
	OYL_CTOR(CreditsLayer, Layer)

	virtual void onEnter() override;

	virtual void onUpdate() override;

	virtual bool onEvent(const oyl::Event& event) override;

	virtual void onGuiRender() override;
};

class CreditsScene : public oyl::Scene
{
public:
	OYL_CTOR(CreditsScene, oyl::Scene)

	virtual void onEnter() override
	{
		pushLayer(CreditsLayer::create());
	}
};