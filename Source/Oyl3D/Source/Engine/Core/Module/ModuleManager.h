#pragma once

#include "ModuleInterface.h"

#include "Core/PPExport.h"
#include "Core/Platform/SharedLibrary.h"

namespace Oyl
{
	using ModuleHandle = struct _ModuleHandle*;

	class ModuleManager
	{
	protected:
		ModuleManager();

		~ModuleManager();

	public:
		CORE_EXPORT
		static
		ModuleManager&
		Get();

		CORE_EXPORT
		IModuleInterface*
		LoadModule(const String& a_moduleName);

		CORE_EXPORT
		IModuleInterface*
		GetModule(const String& a_moduleName) const;

		CORE_EXPORT
		bool
		UnloadModule(const String& a_moduleName);

		template<typename TInterface = IModuleInterface>
			requires (std::is_convertible_v<TInterface*, IModuleInterface*>)
		static
		TInterface*
		LoadModulePointer(const String& a_moduleName)
		{
			return static_cast<TInterface*>(Get().LoadModule(a_moduleName));
		}

		template<typename TInterface = IModuleInterface>
			requires (std::is_convertible_v<TInterface*, IModuleInterface*>)
		static
		TInterface*
		GetModulePointer(const String& a_moduleName)
		{
			return static_cast<TInterface*>(Get().GetModule(a_moduleName));
		}

	private:
		struct Impl;
		Impl* m_impl;
	};
}
