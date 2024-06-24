#pragma once

#include "ModuleRegistry.h"

namespace Oyl
{
	template<typename TModule, ModuleRegistry::enable_if_module_t<TModule>>
	Module*
	ModuleRegistry::RegisterModule()
	{
		Module* module = new TModule();
		m_modules.emplace_back(module);
		module->OnInit();
		return module;
	}

	template<typename TModule, ModuleRegistry::enable_if_module_t<TModule>>
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

	template<typename TModule, ModuleRegistry::enable_if_module_t<TModule>>
	void
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
			return;
		}
		
		Module* module = *moduleIter;
		module->OnShutdown();
		delete module;
		m_modules.erase(moduleIter);
	}
}
