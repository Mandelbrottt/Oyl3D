#include "pch.h"
#include "Time.h"

#include "Core/Profiling/Profiler.h"

#ifdef OYL_WINDOWS
#	include "Time_Windows.h"
#endif

namespace Oyl::Time::Detail
{
	float32 g_deltaTime         = 0.0f;
	float32 g_unscaledDeltaTime = 0.0f;
	float32 g_fixedDeltaTime    = 0.0f;

	float64 g_elapsedTime         = 0.0;
	float64 g_unscaledElapsedTime = 0.0;
	float32 g_timeDifference      = 0.0f;

	float                g_timeScale     = 1.0f;
	std::atomic<float32> g_timeScaleHint = 1.0f;

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
	
	uint64
	TickResolution()
	{
		return Platform::TickResolution();
	}
	
	uint64
	CurrentProcessorTick()
	{
		return Platform::CurrentProcessorTick();
	}

	uint64
	ImmediateElapsedTicks()
	{
		return Platform::ImmediateElapsedTicks();
	}

	float64
	ImmediateElapsedTime()
	{
		return Platform::ImmediateElapsedTime();
	}
}
