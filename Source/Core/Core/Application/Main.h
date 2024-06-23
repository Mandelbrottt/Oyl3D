#pragma once
#include "Core/Common.h"

namespace Oyl
{
	using OnShouldGameUpdateFn = bool(*)();
	using OnApplicationQuitFn = void(*)();
	using OnLogPreInitMessages = void(*)();

	struct CoreInitParameters
	{
		OnShouldGameUpdateFn onShouldGameUpdateCallback;
		OnApplicationQuitFn onApplicationShouldQuitCallback;
		OnLogPreInitMessages onLogPreInitMessages;
	};
	
	OYL_CORE_API
	void
	Init(const CoreInitParameters& a_params);

	OYL_CORE_API
	void
	Update();

	OYL_CORE_API
	void
	Shutdown();
	
	OYL_CORE_API
	bool
	GetShouldGameUpdate() noexcept;

	OYL_CORE_API
	void
	SetShouldGameUpdate(
		bool a_value
	) noexcept;
}
