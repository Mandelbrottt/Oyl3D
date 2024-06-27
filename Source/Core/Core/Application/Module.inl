#pragma once

#include "Module.h"

namespace Oyl
{
	template<typename TModule, typename... TArgs, std::enable_if_t<std::is_base_of_v<TModule, Module>>>
	Module*
	Module::Register(TArgs&&... a_args)
	{
		return ModuleRegistry::RegisterModule<TModule>(std::forward<TArgs>(a_args)...);
	}

	template<typename TModule, std::enable_if_t<std::is_base_of_v<TModule, Module>>>
	Module*
	Module::Get()
	{
		return ModuleRegistry::GetModule<TModule>();
	}

	template<typename TModule, std::enable_if_t<std::is_base_of_v<TModule, Module>>>
	bool
	Module::Remove()
	{
		return ModuleRegistry::RemoveModule<TModule>();
	}
}
