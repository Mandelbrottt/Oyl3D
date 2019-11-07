#pragma once
#include "Oyl3D.h"

struct TimedAction
{
	float elapsed = 0.0f;
	float timeToWait;
};

struct MoveableUsingLerp
{
	glm::vec3 startPos;
	glm::vec3 desinationPos;

	float speed = 0.2f;
	float interpolationParam = 0.0f;
};