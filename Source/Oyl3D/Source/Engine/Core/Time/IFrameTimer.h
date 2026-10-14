#pragma once

#include "Core/Types/PrimitiveTypes.h"

namespace Oyl
{
	class IFrameTimer
	{
	public:
		virtual
		~IFrameTimer() = 0;

		virtual
		void
		Init() = 0;

		virtual
		void
		Update() = 0;

		virtual
		float64
		ElapsedTime() const = 0;

		virtual
		float64
		UnscaledElapsedTime() const = 0;

		virtual
		float32
		DeltaTime() const = 0;

		virtual
		float32
		UnscaledDeltaTime() const = 0;

		virtual
		float32
		FixedDeltaTime() const = 0;

		virtual
		float32
		TimeScale() const = 0;

		virtual
		void
		SetTimeScale(float32 a_scale) = 0;

		virtual
		uint64
		TickResolution() const = 0;

		virtual
		uint64
		CurrentProcessorTick() const = 0;

		virtual
		uint64
		ElapsedProcessorTicks() const = 0;
	};

	inline
	IFrameTimer::~IFrameTimer() {}
}
