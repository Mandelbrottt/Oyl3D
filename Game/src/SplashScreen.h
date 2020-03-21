#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

class SplashScreenLayer : public oyl::Layer
{
public:
	OYL_CTOR(SplashScreenLayer, Layer)

	float changeSceneCountdown = 4.0f;

	virtual void onEnter() override;

	virtual void onUpdate() override;

	virtual bool onEvent(const oyl::Event& event) override;

	virtual void onGuiRender() override;
};

class SplashScreenScene : public oyl::Scene
{
public:
	OYL_CTOR(SplashScreenScene, oyl::Scene)

	virtual void onEnter() override
	{
		pushLayer(SplashScreenLayer::create());
	}
};