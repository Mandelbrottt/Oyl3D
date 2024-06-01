#pragma once
#include "Core/Common.h"

namespace Rearm
{
	using OnShouldGameUpdateFn = bool(*)();
	using OnApplicationQuitFn = void(*)();

	struct CoreInitParameters
	{
		OnShouldGameUpdateFn onShouldGameUpdateCallback;
		OnApplicationQuitFn onApplicationShouldQuitCallback;
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
	
	R_CORE_EXPORT
	bool
	GetShouldGameUpdate() noexcept;

	R_CORE_EXPORT
	void
	SetShouldGameUpdate(
		bool a_value
	) noexcept;
}
