#pragma once
#include "Core/Common.h"

namespace Rearm
{
	using OnShouldGameUpdateFn = bool(*)();

	struct CoreInitParameters
	{
		OnShouldGameUpdateFn onShouldGameUpdateCallback;
	};
	
	REARM_CORE_EXPORT
	void
	Init(
		const CoreInitParameters& a_params
	);

	REARM_CORE_EXPORT
	void
	Update();

	REARM_CORE_EXPORT
	void
	Shutdown();
}
