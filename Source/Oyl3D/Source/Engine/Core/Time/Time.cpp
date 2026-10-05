#include "pch.h"
#include "Time.h"

#include "Core/PPExport.h"
#include "Core/Profiling/Profiling.h"

namespace Oyl::Time::Internal
{
	float32 g_deltaTime = 0.0f;
	float32 g_unscaledDeltaTime = 0.0f;
	float32 g_fixedDeltaTime = 0.0f;

	float64 g_elapsedTime = 0.0;
	float64 g_unscaledElapsedTime = 0.0;
	float32 g_timeDifference = 0.0f;

	float g_timeScale = 1.0f;
	std::atomic<float32> g_timeScaleHint = 1.0f;
}

#if defined(OYL_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace Oyl::Time::Internal
{
	static uint64 g_startTime;
	static uint64 g_lastFrameTime;
	static uint64 g_pcFrequency;

	void
	Init()
	{
		OYL_PROFILE_FUNCTION();

		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		g_startTime = counter.QuadPart;

		LARGE_INTEGER pcFrequency;
		QueryPerformanceFrequency(&pcFrequency);
		g_pcFrequency = pcFrequency.QuadPart;

		g_lastFrameTime = 0;
	}

	void
	Update()
	{
		OYL_PROFILE_FUNCTION();

		g_timeScale = g_timeScaleHint;

		uint64 currentFrameTime = CurrentProcessorTick();
		g_unscaledElapsedTime = (float64) (currentFrameTime - g_startTime) / (float64) g_pcFrequency;
		g_unscaledDeltaTime = (float32) ((currentFrameTime - g_lastFrameTime) / (float64) g_pcFrequency);

		g_deltaTime = g_unscaledDeltaTime * g_timeScale;
		g_timeDifference = g_timeDifference + g_unscaledDeltaTime - g_deltaTime;
		g_elapsedTime = g_unscaledElapsedTime + g_timeDifference;

		g_lastFrameTime = currentFrameTime;
	}

	uint64
	TickResolution()
	{
		return g_pcFrequency;
	}

	uint64
	CurrentProcessorTick()
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return counter.QuadPart;
	}

	uint64
	ImmediateElapsedTicks()
	{
		uint64 currentProcessorTick = CurrentProcessorTick();
		uint64 elapsedTicks = currentProcessorTick - g_startTime;
		return elapsedTicks;
	}

	float64
	ImmediateElapsedTime()
	{
		uint64 elapsedTicks = ImmediateElapsedTicks();
		float64 elapsedTime = (float64) elapsedTicks / (float64) g_pcFrequency;
		return elapsedTime;
	}
}
#endif
