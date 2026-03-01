#pragma once

namespace Oyl::Time
{
	namespace Detail
	{
		extern float32 g_deltaTime;
		extern float32 g_unscaledDeltaTime;
		extern float32 g_fixedDeltaTime;
		
		extern float64 g_elapsedTime;
		extern float64 g_unscaledElapsedTime;
		extern float32 g_timeDifference;

		extern float32 g_timeScale;
		extern std::atomic<float32> g_timeScaleHint;

		void
		OYL_CORE_API
		Init();

		void
		OYL_CORE_API
		Update();

		uint64
		OYL_CORE_API
		TickResolution();

		uint64
		OYL_CORE_API
		CurrentProcessorTick();

		uint64
		OYL_CORE_API
		ImmediateElapsedTicks();

		float64
		OYL_CORE_API
		ImmediateElapsedTime();
	}

	inline
	float64
	ElapsedTime()
	{
		return Detail::g_elapsedTime;
	}
	
	inline
	float64
	UnscaledElapsedTime()
	{
		return Detail::g_unscaledElapsedTime;
	}
	
	inline
	float32
	DeltaTime()
	{
		return Detail::g_deltaTime;
	}
	
	inline
	float32
	UnscaledDeltaTime()
	{
		return Detail::g_unscaledDeltaTime;
	}
	
	inline
	float32
	FixedDeltaTime()
	{
		return Detail::g_fixedDeltaTime;
	}
	
	inline
	float32
	SmoothDeltaTime()
	{
		return Detail::g_deltaTime;
	}
	
	inline
	void
	SetTimeScale(float32 a_scale)
	{
		Detail::g_timeScaleHint = a_scale;
	}
}
