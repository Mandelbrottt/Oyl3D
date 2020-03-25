#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

#include "MainGameObjectsInit.h"
#include "MainGameGuiInit.h"

class TutorialLayer : public oyl::Layer
{
public:
	OYL_CTOR(TutorialLayer, Layer)

	bool firstFrame = true;

	bool initSegment = true;

	float segmentTimer1;
	float segmentTimer2;
	float segmentTimer3;

	float segmentInterpolationParam1;
	float segmentInterpolationParam2;
	float segmentInterpolationParam3;

	virtual void onEnter() override;

	virtual void onUpdate() override;

	virtual bool onEvent(const oyl::Event& event) override;

	virtual void onGuiRender() override;
};

class TutorialScene : public oyl::Scene
{
public:
	OYL_CTOR(TutorialScene, oyl::Scene)

	virtual void onEnter() override
	{
		pushLayer(TutorialLayer::create());
		pushLayer(MainGameObjectsInitLayer::create());
		pushLayer(MainGameGuiInitLayer::create());
	}
};