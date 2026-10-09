#include "WindowsFrameTimer.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace Oyl
{
	void
	WindowsFrameTimer::Init()
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		m_startTick = counter.QuadPart;

		LARGE_INTEGER pcFrequency;
		QueryPerformanceFrequency(&pcFrequency);
		m_perfCounterFrequency = pcFrequency.QuadPart;

		m_lastFrameTick = m_startTick;
	}

	void
	WindowsFrameTimer::Update()
	{
		m_timeScale = m_timeScaleHint;

		uint64 currentFrameTick = CurrentProcessorTick();
		m_unscaledDeltaTime = (float32) ((currentFrameTick - m_lastFrameTick) / (float64) m_perfCounterFrequency);
		m_unscaledElapsedTime = (float64) (currentFrameTick - m_startTick) / (float64) m_perfCounterFrequency;

		m_deltaTime = m_unscaledDeltaTime * m_timeScale;
		m_timeDifference += m_unscaledDeltaTime - m_deltaTime;
		m_elapsedTime = m_unscaledElapsedTime + m_timeDifference;

		m_lastFrameTick = currentFrameTick;
	}

	uint64
	WindowsFrameTimer::TickResolution() const
	{
		return m_perfCounterFrequency;
	}

	uint64
	WindowsFrameTimer::CurrentProcessorTick() const
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return counter.QuadPart;
	}

	uint64
	WindowsFrameTimer::ElapsedProcessorTicks() const
	{
		return CurrentProcessorTick() - m_startTick;
	}
}
