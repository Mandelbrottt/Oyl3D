#pragma once

#include <Windows.h>

namespace Oyl::Time::Platform
{
	static uint64 g_startTime;
	static uint64 g_lastFrameTime;
	static uint64 g_pcFrequency;

	constexpr int NUM_FRAMES_TO_HOLD = 10;
	static float  g_prevFrameTimes[NUM_FRAMES_TO_HOLD] { 0.0f };
	static int    g_prevFrameIndex;

	static
	void
	Init()
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		g_startTime = counter.QuadPart;

		LARGE_INTEGER pcFrequency;
		QueryPerformanceFrequency(&pcFrequency);
		g_pcFrequency = pcFrequency.QuadPart;

		g_lastFrameTime = 0;
	}

	static
	void
	Update()
	{
		using namespace Detail;

		g_timeScale = g_timeScaleHint;

		uint64 currentFrameTime = CurrentProcessorTick();
		g_unscaledElapsedTime = (float64) (currentFrameTime - g_startTime) / (float64) g_pcFrequency;
		g_unscaledDeltaTime = (float32) ((currentFrameTime - g_lastFrameTime) / (float64) g_pcFrequency);

		// Smoothing unscaled delta time
		//g_prevFrameTimes[g_prevFrameIndex++] = g_unscaledDeltaTime;
		//float sum = 0.0f;
		//int numCounts = 0;
		//for (float frameTime : g_prevFrameTimes)
		//{
		//	sum += frameTime;
		//	numCounts += frameTime == 0.0f ? 0 : 1;
		//}
		//sum /= static_cast<float>(numCounts);
		//g_prevFrameIndex %= 10;

		//g_unscaledDeltaTime = sum;

		//m_unscaledDeltaTime = glm::clamp(static_cast<float>(currentTime), 0.0001f, 0.1f);
		//m_unscaledDeltaTime = glm::max(static_cast<float>(currentTime), 0.0f);
		
		g_deltaTime = g_unscaledDeltaTime * g_timeScale;
		g_timeDifference = g_timeDifference + g_unscaledDeltaTime - g_deltaTime;
		g_elapsedTime = g_unscaledElapsedTime + g_timeDifference;

		g_lastFrameTime = currentFrameTime;
	}

	static
	uint64
	TickResolution()
	{
		return g_pcFrequency;
	}
	
	static
	uint64
	CurrentProcessorTick()
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return counter.QuadPart;
	}
	
	static
	uint64
	ImmediateElapsedTicks()
	{
		uint64 currentProcessorTick = CurrentProcessorTick();
		uint64 elapsedTicks = currentProcessorTick - g_startTime;
		return elapsedTicks;
	}
	
	static
	float64
	ImmediateElapsedTime()
	{
		uint64 elapsedTicks = ImmediateElapsedTicks();
		float64 elapsedTime = (float64) elapsedTicks / (float64) g_pcFrequency;
		return elapsedTime;
	}
}
