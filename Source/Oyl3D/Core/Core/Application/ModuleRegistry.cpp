#include "pch.h"
#include "ModuleRegistry.h"

#include "Main.h"
#include "Module.h"

namespace Oyl
{
	ModuleRegistry*
	ModuleRegistry::Instance()
	{
		return Oyl::Detail::GetModuleRegistry();
	}
	
	Module*
	ModuleRegistry::GetModule(TypeId a_typeId)
	{
		Module* module     = nullptr;
		auto    moduleIter = std::find_if(
			begin(),
			end(),
			[a_typeId](Module* a_module) { return a_module->GetTypeId() == a_typeId; }
		);

		if (moduleIter != end())
		{
			module = *moduleIter;
		}

		return module;
	}

	bool
	ModuleRegistry::RemoveModule(TypeId a_typeId)
	{
		auto moduleIter = std::find_if(
			begin(),
			end(),
			[a_typeId](Module* a_module) { return a_module->GetTypeId() == a_typeId; }
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
