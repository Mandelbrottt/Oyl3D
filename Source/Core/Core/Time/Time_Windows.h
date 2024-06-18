#pragma once

#include <Windows.h>

namespace Oyl::Time::Platform
{
	static LARGE_INTEGER g_startTime;
	static LARGE_INTEGER g_lastFrameTime;
	static double g_pcFrequency;

	constexpr int NUM_FRAMES_TO_HOLD = 10;
	static float g_prevFrameTimes[NUM_FRAMES_TO_HOLD] { 0.0f };
	static int g_prevFrameIndex;
	
	static
	void
	Init()
	{
		QueryPerformanceCounter(&g_startTime);

		LARGE_INTEGER pcFrequency;
		QueryPerformanceFrequency(&pcFrequency);
		g_pcFrequency = static_cast<double>(pcFrequency.QuadPart);

		ZeroMemory(&g_lastFrameTime, sizeof(g_lastFrameTime));
	}

	static
	void
	Update()
	{
		using namespace Detail;
		
        g_timeScale = g_timeScaleHint;

        LARGE_INTEGER currentFrameTime;
        QueryPerformanceCounter(&currentFrameTime);

		double doubleUnscaledElapsedTime = 
			static_cast<double>(currentFrameTime.QuadPart - g_startTime.QuadPart) / g_pcFrequency;
		g_unscaledElapsedTime = static_cast<float>(doubleUnscaledElapsedTime);

        double currentTime = static_cast<double>(currentFrameTime.QuadPart - g_lastFrameTime.QuadPart) / g_pcFrequency;

        g_prevFrameTimes[g_prevFrameIndex++] = std::clamp(static_cast<float>(currentTime), 0.0f, 0.1f);
        float sum = 0.0f;
        int numCounts = 0;
		for (float frameTime : g_prevFrameTimes)
		{
			sum += frameTime;
			numCounts += frameTime == 0.0f ? 0 : 1;
		}
        sum /= static_cast<float>(numCounts);
        g_prevFrameIndex %= 10;

        g_unscaledDeltaTime = sum;
        
        //m_unscaledDeltaTime = glm::clamp(static_cast<float>(currentTime), 0.0001f, 0.1f);
        //m_unscaledDeltaTime = glm::max(static_cast<float>(currentTime), 0.0f);
        g_deltaTime = g_unscaledDeltaTime * g_timeScale;

        g_timeDifference = g_timeDifference + g_unscaledDeltaTime - g_deltaTime;

        g_elapsedTime = g_unscaledElapsedTime + g_timeDifference;

        g_lastFrameTime = currentFrameTime;
	}
}
