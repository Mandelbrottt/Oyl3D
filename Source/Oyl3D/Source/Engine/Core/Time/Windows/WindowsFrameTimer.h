#pragma once

#include <atomic>

#include "Core/Time/IFrameTimer.h"

namespace Oyl
{
	class CORE_EXPORT WindowsFrameTimer : public IFrameTimer
	{
	public:
		virtual
		~WindowsFrameTimer() = default;

		void
		Init() override;

		void
		Update() override;

		float64
		ElapsedTime() const override { return m_elapsedTime; }

		float64
		UnscaledElapsedTime() const override { return m_unscaledElapsedTime; }

		float32
		DeltaTime() const override { return m_deltaTime; }

		float32
		UnscaledDeltaTime() const override { return m_unscaledDeltaTime; }

		float32
		FixedDeltaTime() const override { return m_fixedDeltaTime; }

		float32
		TimeScale() const override { return m_timeScale; }

		void
		SetTimeScale(float32 a_scale) override { m_timeScaleHint = a_scale; }

		uint64
		TickResolution() const override;

		uint64
		CurrentProcessorTick() const override;

		uint64
		ElapsedProcessorTicks() const override;

	private:
		float32 m_deltaTime = 0;
		float32 m_unscaledDeltaTime = 0;
		float32 m_fixedDeltaTime = 0;

		float64 m_elapsedTime = 0;
		float64 m_unscaledElapsedTime = 0;
		float64 m_timeDifference = 0;

		float32 m_timeScale = 0;
		std::atomic<float32> m_timeScaleHint = 0;

		uint64 m_startTick = 0;
		uint64 m_lastFrameTick = 0;
		uint64 m_perfCounterFrequency = 0;
	};
}
