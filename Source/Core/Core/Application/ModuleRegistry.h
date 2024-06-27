#pragma once

#include "Core/Common.h"

namespace Oyl
{
	class Module;

	class OYL_CORE_API ModuleRegistry
	{
		using ModuleList = std::vector<Module*>;

	public:
		template<typename TModule, typename... TArgs, std::enable_if_t<std::is_base_of_v<Module, TModule>> = true>
		Module*
		RegisterModule(TArgs&&... a_args);

		template<typename TModule, std::enable_if_t<std::is_base_of_v<Module, TModule>> = true>
		Module*
		GetModule();

		template<typename TModule, std::enable_if_t<std::is_base_of_v<Module, TModule>> = true>
		bool
		RemoveModule();
		
		ModuleList::iterator begin() { return m_modules.begin(); }

		ModuleList::iterator end() { return m_modules.end(); }

		ModuleList::reverse_iterator rbegin() { return m_modules.rbegin(); }

		ModuleList::reverse_iterator rend() { return m_modules.rend(); }

	private:
		std::vector<Module*> m_modules;
	};
}

#include "ModuleRegistry.inl"