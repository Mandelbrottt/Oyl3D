#include "pch.h"
#include "Time.h"

#include "Core/Profiling/Profiler.h"

#ifdef OYL_WINDOWS
#	include "Time_Windows.h"
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
		// Currently can't profile init function, since profiler relies on Time class
		// TODO: Decouple Profiler from Time
		return Platform::Init();
	}

	void
	Update()
	{
		OYL_PROFILE_FUNCTION();
		return Platform::Update();
	}

	double
	ImmediateElapsedTime()
	{
		return Platform::ImmediateElapsedTime();
	}
	
	uint64
	CurrentProcessorTick()
	{
		return Platform::CurrentProcessorTick();
	}
}
