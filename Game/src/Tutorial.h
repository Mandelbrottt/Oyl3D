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
	float segmentTimer9;

	bool segmentBool1;
	bool segmentBool2;
	bool segmentBool3;
	bool segmentBool4;
	bool segmentBool5;
	bool segmentBool6;
	bool segmentBool7;
	bool segmentBool8;
	bool segmentBool9;

	float segmentInterpolationParam1;
	float segmentInterpolationParam2;
	float segmentInterpolationParam3;

	entt::entity tutPlayerEntity;
	entt::entity tutCameraEntity;

	bool firstSegmentInit;
	bool secondSegmentInit;
	bool thirdSegmentInit;
	bool fourthSegmentInit;
	bool fifthSegmentInit;
	bool sixthSegmentInit;

	Player* playerSegment1;
	glm::quat playerSegment1Rot;
	glm::vec3 playerSegment1Pos;
	glm::quat cameraSegment1Rot;

	Player* playerSegment2;
	glm::quat playerSegment2Rot;
	glm::vec3 playerSegment2Pos;
	glm::quat cameraSegment2Rot;

	Player* playerSegment3;
	glm::quat playerSegment3Rot;
	glm::vec3 playerSegment3Pos;
	glm::quat cameraSegment3Rot;

	Player* playerSegment4;
	glm::quat playerSegment4Rot;
	glm::vec3 playerSegment4Pos;
	glm::quat cameraSegment4Rot;

	Player* playerSegment5;
	glm::quat playerSegment5Rot;
	glm::vec3 playerSegment5Pos;
	glm::quat cameraSegment5Rot;

	Player* playerSegment6;
	glm::quat playerSegment6Rot;
	glm::vec3 playerSegment6Pos;
	glm::quat cameraSegment6Rot;

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

	void moveToNextSegment();
	void repeatSegment();

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