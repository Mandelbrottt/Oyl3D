#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

#include "MainGameObjectsInit.h"
#include "MainGameGuiInit.h"
#include "ScrollingTextureLayer.h"

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
	float segmentTimer4;
	float segmentTimer5;
	float segmentTimer6;
	float segmentTimer7;
	float segmentTimer8;

	bool segmentBool1;
	bool segmentBool2;
	bool segmentBool3;
	bool segmentBool4;
	bool segmentBool5;
	bool segmentBool6;
	bool segmentBool7;
	bool segmentBool8;

	float segmentFloat1;
	float segmentFloat2;
	float segmentFloat3;
	float segmentFloat4;
	float segmentFloat5;
	float segmentFloat6;
	float segmentFloat7;
	float segmentFloat8;

	float segmentInterpolationParam1;
	float segmentInterpolationParam2;
	float segmentInterpolationParam3;

	entt::entity tutPlayerEntity;
	entt::entity tutCameraEntity;

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

	void movePlayerToPos(glm::vec3 a_targetPos, bool* a_isFinished);

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
		pushLayer(ScrollingTextureLayer::create());
	}
};