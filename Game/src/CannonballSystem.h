#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class CannonballSystem : public System
{
	OYL_CTOR(CannonballSystem, System)

	std::vector<glm::vec3> blueFrontSplines;
	std::vector<glm::vec3> blueMiddleSplines;
	std::vector<glm::vec3> blueBackSplines;

	std::vector<glm::vec3> redFrontSplines;
	std::vector<glm::vec3> redMiddleSplines;
	std::vector<glm::vec3> redBackSplines;

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void setCannonballToCarriedForPlayer(entt::entity a_playerEntity, entt::entity a_cannonballEntity);
};