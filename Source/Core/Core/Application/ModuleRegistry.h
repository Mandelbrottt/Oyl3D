#pragma once

#include "Module.h"
#include "Core/Common.h"

namespace Oyl
{
	class OYL_CORE_API ModuleRegistry
	{
		using ModuleList = std::vector<Module*>;

		template<typename TModule>
		using is_module_t = std::enable_if_t<std::is_base_of_v<Module, TModule>>;

	public:
		template<typename TModule, typename = is_module_t<TModule>>
		Module*
		RegisterModule();

		template<typename TModule, typename = is_module_t<TModule>>
		Module*
		GetModule();

		template<typename TModule, typename = is_module_t<TModule>>
		void
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