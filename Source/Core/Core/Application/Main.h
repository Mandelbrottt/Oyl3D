#pragma once
#include "Core/Common.h"

namespace Rearm
{
	using OnShouldGameUpdateFn = bool(*)();

	struct CoreInitParameters
	{
		OnShouldGameUpdateFn onShouldGameUpdateCallback;
	};
	
	R_CORE_EXPORT
	void
	Init(
		const CoreInitParameters& a_params
	);

	R_CORE_EXPORT
	void
	Update();

	R_CORE_EXPORT
	void
	Shutdown();
}
