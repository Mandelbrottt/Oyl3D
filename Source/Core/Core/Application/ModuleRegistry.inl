#pragma once

#include "ModuleRegistry.h"

namespace Oyl
{
	template<typename TModule, typename... TArgs, std::enable_if_t<std::is_base_of_v<Module, TModule>>>
	Module*
	ModuleRegistry::RegisterModule(TArgs&&... a_args)
	{
		Module* module = new TModule(std::forward<TArgs>(a_args)...);
		m_modules.emplace_back(module);
		module->OnInit();
		return module;
	}

	template<typename TModule, std::enable_if_t<std::is_base_of_v<Module, TModule>>>
	Module*
	ModuleRegistry::GetModule()
	{
		Module* module       = nullptr;
		auto    typeIdToFind = GetTypeId<TModule>();
		auto    moduleIter   = std::find_if(
			begin(),
			end(),
			[typeIdToFind](Module* a_module) { return a_module->GetTypeId() == typeIdToFind; }
		);
		
		if (moduleIter != end())
		{
			module = *moduleIter;
		}

		return module;
	}

	template<typename TModule, std::enable_if_t<std::is_base_of_v<Module, TModule>>>
	bool
	ModuleRegistry::RemoveModule()
	{
		auto typeIdToRemove = GetTypeId<TModule>();
		auto moduleIter     = std::find_if(
			begin(),
			end(),
			[typeIdToRemove](Module* a_module) { return a_module->GetTypeId() == typeIdToRemove; }
		);
		
		if (moduleIter == end())
		{
			return false;
		}
		
		Module* module = *moduleIter;
		module->OnShutdown();
		delete module;
		m_modules.erase(moduleIter);
		return true;
	}
}
