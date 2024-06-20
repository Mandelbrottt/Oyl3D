#pragma once

namespace Oyl::Time
{
	namespace Detail
	{
		extern float g_deltaTime;
		extern float g_unscaledDeltaTime;
		extern float g_fixedDeltaTime;
		
		extern float g_elapsedTime;
		extern float g_unscaledElapsedTime;
		extern float g_timeDifference;

		extern float g_timeScale;
		extern std::atomic<float> g_timeScaleHint;

		void
		OYL_CORE_API
		Init();

		void
		OYL_CORE_API
		Update();

		u64
		OYL_CORE_API
		CurrentProcessorTick();

		double
		OYL_CORE_API
		ImmediateElapsedTime();
	}

	inline
	float
	ElapsedTime()
	{
		return Detail::g_elapsedTime;
	}
	
	inline
	float
	UnscaledElapsedTime()
	{
		return Detail::g_unscaledElapsedTime;
	}
	
	inline
	float
	DeltaTime()
	{
		return Detail::g_deltaTime;
	}
	
	inline
	float
	UnscaledDeltaTime()
	{
		return Detail::g_unscaledDeltaTime;
	}
	
	inline
	float
	FixedDeltaTime()
	{
		return Detail::g_fixedDeltaTime;
	}
	
	inline
	float
	SmoothDeltaTime()
	{
		return Detail::g_deltaTime;
	}
	
	inline
	void
	SetTimeScale(float a_scale)
	{
		Detail::g_timeScaleHint = a_scale;
	}
}
