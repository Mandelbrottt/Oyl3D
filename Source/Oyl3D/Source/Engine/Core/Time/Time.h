#pragma once

#include <atomic>

#include "Core/Types/PrimitiveTypes.h"

namespace Oyl::Time
{
	namespace Internal
	{
		CORE_EXPORT extern float32 g_deltaTime;
		CORE_EXPORT extern float32 g_unscaledDeltaTime;
		CORE_EXPORT extern float32 g_fixedDeltaTime;

		CORE_EXPORT extern float64 g_elapsedTime;
		CORE_EXPORT extern float64 g_unscaledElapsedTime;
		CORE_EXPORT extern float32 g_timeDifference;

		CORE_EXPORT extern float32 g_timeScale;
		CORE_EXPORT extern std::atomic<float32> g_timeScaleHint;
	}

	inline
	float64
	ElapsedTime()
	{
		return Internal::g_elapsedTime;
	}

	inline
	float64
	UnscaledElapsedTime()
	{
		return Internal::g_unscaledDeltaTime;
	}

	inline
	float32
	DeltaTime()
	{
		return Internal::g_deltaTime;
	}

	inline
	float32
	UnscaledDeltaTime()
	{
		return Internal::g_unscaledDeltaTime;
	}

	inline
	float32
	FixedDeltaTime()
	{
		return Internal::g_fixedDeltaTime;
	}

	inline
	void
	SetTimeScale(float32 a_scale)
	{
		Internal::g_timeScaleHint = a_scale;
	}

	namespace Internal
	{
		CORE_EXPORT
		void
		Init();

		void
		CORE_EXPORT
		Update();

		uint64
		CORE_EXPORT
		TickResolution();

		uint64
		CORE_EXPORT
		CurrentProcessorTick();

		uint64
		CORE_EXPORT
		ImmediateElapsedTicks();

		float64
		CORE_EXPORT
		ImmediateElapsedTime();
	}
}
