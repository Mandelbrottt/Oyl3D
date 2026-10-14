#pragma once

#include "Core/Preprocessor/Common.h"
#include "Core/Time/IFrameTimer.h"
#include "Core/Types/PrimitiveTypes.h"

namespace Oyl::Time
{
	CORE_EXPORT
	float32
	DeltaTime();

	CORE_EXPORT
	float64
	ElapsedTime();

	CORE_EXPORT
	float32
	UnscaledDeltaTime();

	CORE_EXPORT
	float64
	UnscaledElapsedTime();

	CORE_EXPORT
	float32
	FixedDeltaTime();

	CORE_EXPORT
	float32
	TimeScale();

	CORE_EXPORT
	void
	SetTimeScale(float32 a_scale);

	namespace Internal
	{
		CORE_EXPORT
		void
		Init(IFrameTimer* a_defaultFrameTimer);

		void
		CORE_EXPORT
		Update();

		CORE_EXPORT
		IFrameTimer*
		GetFrameTimer();

		CORE_EXPORT
		void
		SetFrameTimer(IFrameTimer* a_frameTimer);
	}
}
