#include "pch.h"
#include "Time.h"

#ifdef OYL_WINDOWS
#	include "Core/Platform/Windows/Time/Time.h"
#endif

namespace Oyl::Time::Detail
{
	float g_deltaTime         = 0.0f;
	float g_unscaledDeltaTime = 0.0f;
	float g_fixedDeltaTime    = 0.0f;

	float g_elapsedTime         = 0.0f;
	float g_unscaledElapsedTime = 0.0f;
	float g_timeDifference      = 0.0f;

	float              g_timeScale     = 1.0f;
	std::atomic<float> g_timeScaleHint = 1.0f;

	void
	Init()
	{
		return Platform::Init();
	}

	void
	Update()
	{
		return Platform::Update();
	}

	double
	ImmediateElapsedTime()
	{
		return Platform::ImmediateElapsedTime();
	}
}
