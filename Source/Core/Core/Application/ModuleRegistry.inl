#pragma once

#include "ModuleRegistry.h"

namespace Oyl
{
	template<typename TModule, typename... TArgs, ModuleRegistry::enable_if_base_of_module_t<TModule>>
	bool
	ModuleRegistry::RegisterModule(TArgs&&... a_args)
	{
		TModule* module = new TModule(std::forward<TArgs>(a_args)...);
		return RegisterModule(module);
	}

	template<typename TModule, ModuleRegistry::enable_if_base_of_module_t<TModule>>
	TModule*
	ModuleRegistry::GetModule()
	{
		Module* module = GetModule(TModule::GetStaticTypeId());
		return reinterpret_cast<TModule*>(module);
	}

	template<typename TModule, ModuleRegistry::enable_if_base_of_module_t<TModule>>
	bool
	ModuleRegistry::RemoveModule()
	{
		return RemoveModule(TModule::GetStaticTypeId());
	}
}
