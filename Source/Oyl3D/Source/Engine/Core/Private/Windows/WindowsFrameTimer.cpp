#include "WindowsFrameTimer.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace Oyl
{
	void
	WindowsFrameTimer::Init()
	{
		LARGE_INTEGER pcFrequency;
		QueryPerformanceFrequency(&pcFrequency);
		m_perfCounterFrequency = static_cast<float64>(pcFrequency.QuadPart);

		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		m_startTime = counter.QuadPart / m_perfCounterFrequency;

		m_lastFrameTime = m_startTime;
	}

	void
	WindowsFrameTimer::Update()
	{
		m_timeScale = m_timeScaleHint;

		float64 currentFrameTime = CurrentProcessorTime();
		m_unscaledDeltaTime = static_cast<float32>(currentFrameTime - m_lastFrameTime);
		m_unscaledElapsedTime = currentFrameTime - m_startTime;

		m_deltaTime = m_unscaledDeltaTime * m_timeScale;
		m_timeDifference += m_unscaledDeltaTime - m_deltaTime;
		m_elapsedTime = m_unscaledElapsedTime + m_timeDifference;

		m_lastFrameTime = currentFrameTime;
	}

	float64
	WindowsFrameTimer::CurrentProcessorTime() const
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return counter.QuadPart / m_perfCounterFrequency;
	}

	float64
	WindowsFrameTimer::ElapsedProcessorTime() const
	{
		return CurrentProcessorTime() - m_startTime;
	}
}
