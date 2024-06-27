#pragma once

#include "Core/Common.h"

namespace Oyl
{
	class ModuleRegistry;
	struct Event;
}

namespace Oyl::Detail
{
	using OnShouldGameUpdateFn = bool(*)();
	using OnApplicationQuitFn = void(*)();

	struct CoreInitParameters
	{
		OnShouldGameUpdateFn onShouldGameUpdateCallback;
		OnApplicationQuitFn onApplicationShouldQuitCallback;
	};
	
	OYL_CORE_API
	void
	Init(const CoreInitParameters& a_params);

	OYL_CORE_API
	void
	Update();
	
	OYL_CORE_API
	void
	OnEvent(Event& a_event);

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

	OYL_CORE_API
	ModuleRegistry*
	GetModuleRegistry();
}
