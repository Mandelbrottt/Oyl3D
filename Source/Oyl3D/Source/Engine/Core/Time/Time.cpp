#include "Time.h"

#include <atomic>

#include "IFrameTimer.h"

#include "Core/Profiling/Profiling.h"

namespace Oyl::Time
{
	static IFrameTimer* g_defaultFrameTimer = nullptr;
	static IFrameTimer* g_frameTimer = nullptr;

	static float32 g_deltaTime = 0.0f;
	static float32 g_unscaledDeltaTime = 0.0f;
	static float32 g_fixedDeltaTime = 0.0f;

	static float64 g_elapsedTime = 0.0;
	static float64 g_unscaledElapsedTime = 0.0;

	static float g_timeScale = 1.0f;
	static std::atomic<float32> g_timeScaleHint = 1.0f;

	float32
	DeltaTime()
	{
		return g_deltaTime;
	}

	float64
	ElapsedTime()
	{
		return g_elapsedTime;
	}

	float32
	UnscaledDeltaTime()
	{
		return g_unscaledDeltaTime;
	}

	float64
	UnscaledElapsedTime()
	{
		return g_unscaledElapsedTime;
	}

	float32
	FixedDeltaTime()
	{
		return g_fixedDeltaTime;
	}

	float32
	TimeScale()
	{
		return g_timeScale;
	}

	void
	SetTimeScale(float32 a_scale)
	{
		g_timeScaleHint = a_scale;
	}

	namespace Internal
	{
		void
		Init(IFrameTimer* a_defaultFrameTimer)
		{
			OYL_PROFILE_FUNCTION();

			assert(a_defaultFrameTimer);

			g_defaultFrameTimer = a_defaultFrameTimer;
			SetFrameTimer(g_defaultFrameTimer);

			g_frameTimer->Init();
		}

		void
		Update()
		{
			OYL_PROFILE_FUNCTION();

			g_frameTimer->SetTimeScale(g_timeScaleHint);
			g_frameTimer->Update();

			g_timeScale = g_frameTimer->TimeScale();
			g_unscaledDeltaTime = g_frameTimer->UnscaledDeltaTime();
			g_unscaledElapsedTime = g_frameTimer->UnscaledElapsedTime();
			g_deltaTime = g_frameTimer->DeltaTime();
			g_elapsedTime = g_frameTimer->ElapsedTime();
			g_fixedDeltaTime = g_frameTimer->FixedDeltaTime();
		}

		IFrameTimer*
		GetFrameTimer()
		{
			return g_frameTimer;
		}

		void
		SetFrameTimer(IFrameTimer* a_frameTimer)
		{
			assert(g_defaultFrameTimer);

			if (a_frameTimer != nullptr)
				g_frameTimer = a_frameTimer;
			else // If user supplies nullptr, use the default frame timer
				g_frameTimer = g_defaultFrameTimer;
		}
	}
}
