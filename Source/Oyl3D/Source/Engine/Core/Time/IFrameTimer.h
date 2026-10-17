#pragma once

#include "Core/Types/Primitives.h"

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
		float64
		CurrentProcessorTime() const = 0;

		virtual
		float64
		ElapsedProcessorTime() const = 0;
	};

	inline
	IFrameTimer::~IFrameTimer() {}
}
