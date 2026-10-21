#include "Time.h"

#include <atomic>

#include "Core/Profiling/Profiling.h"

#if defined(OYL_WINDOWS)
#include "Windows/WindowsFrameTimer.h"
static Oyl::WindowsFrameTimer g_defaultFrameTimer;
#endif

namespace Oyl::Time
{
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
		Init()
		{
			OYL_PROFILE_FUNCTION();

			g_defaultFrameTimer.Init();

			// If user hasn't set a FrameTimer, use the default
			if (g_frameTimer == nullptr)
			{
				SetFrameTimer(&g_defaultFrameTimer);
			}

			// Don't initialize the frame timer if none was provided, since we always initialize the default timer
			if (g_frameTimer != &g_defaultFrameTimer)
			{
				g_frameTimer->Init();
			}
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

		const IFrameTimer&
		GetFrameTimer()
		{
			OYL_ASSERT(g_frameTimer);
			return *g_frameTimer;
		}

		void
		SetFrameTimer(IFrameTimer* a_frameTimer)
		{
			if (a_frameTimer != nullptr)
				g_frameTimer = a_frameTimer;
			else // If user supplies nullptr, use the default frame timer
				g_frameTimer = &g_defaultFrameTimer;
		}
	}
}
