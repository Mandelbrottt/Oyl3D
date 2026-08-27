#pragma once

#include "Core/Common.h"
#include "Core/Events/Event.h"
#include "Core/Events/EventDispatcher.h"
#include "Core/Events/EventListener.h"
#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	void
	ApplicationInit();

	class OYL_CORE_API ModuleRegistry
	{
		friend class Module;
		friend void
		Oyl::ApplicationInit();

		using ModuleList = std::vector<Module*>;

		static ModuleRegistry* s_currentModuleRegistry;

	public:
		static
		ModuleRegistry*
		GetCurrent();

		Module*
		RegisterModule(Module* a_module);

		template<typename TModule, typename... TArgs>
			requires std::is_assignable_v<Traits::TAddPointer<Module>&, Traits::TAddPointer<TModule>>
		TModule*
		RegisterModule(TArgs&&... a_args)
		{
			TModule* module = new TModule(std::forward<TArgs>(a_args)...);
			RegisterModule(module);
			return module;
		}

		template<typename TModule>
			requires std::is_assignable_v<Traits::TAddPointer<Module>&, Traits::TAddPointer<TModule>>
		TModule*
		GetModule()
		{
			Module* module = GetModule(Reflection::GetTypeId<TModule>());
			return reinterpret_cast<TModule*>(module);
		}

		Module*
		GetModule(Reflection::TypeId a_typeId);

		bool
		RemoveModule(Reflection::TypeId a_typeId);

		template<typename TModule>
			requires std::is_assignable_v<Traits::TAddPointer<Module>&, Traits::TAddPointer<TModule>>
		bool
		RemoveModule()
		{
			return RemoveModule(Reflection::GetTypeId<TModule>());
		}

		ModuleList::iterator
		begin() { return m_modules.begin(); }

		ModuleList::iterator
		end() { return m_modules.end(); }

		ModuleList::reverse_iterator
		rbegin() { return m_modules.rbegin(); }

		ModuleList::reverse_iterator
		rend() { return m_modules.rend(); }

	private:
		std::vector<Module*> m_modules;

		EventDispatcher m_eventDispatcher;
	};
}

//#include "ModuleRegistry.inl"
