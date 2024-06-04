#pragma once
#include "Core/Common.h"

namespace Oyl
{
	using OnShouldGameUpdateFn = bool(*)();
	using OnApplicationQuitFn = void(*)();

	struct CoreInitParameters
	{
		OnShouldGameUpdateFn onShouldGameUpdateCallback;
		OnApplicationQuitFn onApplicationShouldQuitCallback;
	};
	
	OYL_CORE_EXPORT
	void
	Init(
		const CoreInitParameters& a_params
	);

	OYL_CORE_EXPORT
	void
	Update();

	OYL_CORE_EXPORT
	void
	Shutdown();
	
	OYL_CORE_EXPORT
	bool
	GetShouldGameUpdate() noexcept;

	OYL_CORE_EXPORT
	void
	SetShouldGameUpdate(
		bool a_value
	) noexcept;
}
