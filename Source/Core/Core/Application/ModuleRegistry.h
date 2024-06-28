#pragma once

#include "Core/Application/Main.h"
#include "Core/Common.h"
#include "Core/Events/Event.h"
#include "Core/Types/TypeId.h"

namespace Oyl
{
	class Module;

	class OYL_CORE_API ModuleRegistry
	{
		friend class Module;

		using ModuleList = std::vector<Module*>;

		template<typename TModule>
		using enable_if_base_of_module_t = std::enable_if_t<std::is_base_of_v<Module, TModule>, bool>;

	public:
		static
		ModuleRegistry*
		Instance();

		bool
		RegisterModule(Module* a_module);

		template<typename TModule, typename... TArgs, enable_if_base_of_module_t<TModule> = true>
		bool
		RegisterModule(TArgs&&... a_args)
		{
			TModule* module = new TModule(std::forward<TArgs>(a_args)...);
			return RegisterModule(module);
		}

		Module*
		GetModule(TypeId a_typeId);

		template<typename TModule, enable_if_base_of_module_t<TModule> = true>
		TModule*
		GetModule()
		{
			Module* module = GetModule(TModule::GetStaticTypeId());
			return reinterpret_cast<TModule*>(module);
		}

		bool
		RemoveModule(TypeId a_typeId);

		template<typename TModule, enable_if_base_of_module_t<TModule> = true>
		bool
		RemoveModule()
		{
			return RemoveModule(TModule::GetStaticTypeId());
		}

		void
		SetOnEventCallback(OnEventFn a_fn) { m_onEventCallback = a_fn; }

		ModuleList::iterator begin() { return m_modules.begin(); }

		ModuleList::iterator end() { return m_modules.end(); }

		ModuleList::reverse_iterator rbegin() { return m_modules.rbegin(); }

		ModuleList::reverse_iterator rend() { return m_modules.rend(); }

	private:
		std::vector<Module*> m_modules;

		OnEventFn m_onEventCallback;
	};
}

//#include "ModuleRegistry.inl"
