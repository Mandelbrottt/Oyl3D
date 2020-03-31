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

	enum class TutorialSegment
	{
		intro,
		segment1,
		segment2,
		segment3,
		segment4,
		segment5,
		segment6,
		outro
	};

	TutorialSegment currentSegment;

	bool firstFrame        = true;
	bool initSegment       = true;
	bool isSegmentFinished = false;

	float segmentTimer1;
	float segmentTimer2;
	float segmentTimer3;

	float segmentInterpolationParam1;
	float segmentInterpolationParam2;
	float segmentInterpolationParam3;

	entt::entity tutPlayerEntity;

	virtual void onEnter() override;

	virtual void onUpdate() override;

	virtual bool onEvent(const oyl::Event& event) override;

	virtual void onGuiRender() override;

	void intro();
	void segment1();
	void segment2();
	void segment3();
	void segment4();
	void segment5();
	void segment6();
	void outro();

	void (TutorialLayer::*currentSegmentFunc)();

	struct TutorialEndSegmentPrompt {};
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