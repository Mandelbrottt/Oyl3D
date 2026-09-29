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

#define OYL_MODULE_INIT_FN_NAME(_module_name_) ModuleInit_##_module_name_
#define OYL_MODULE_DEPS_FN_NAME(_module_name_) ModuleDeps_##_module_name_
#define OYL_MODULE_DEPS_MACRO(_module_name_) _module_name_

#define EXPAND(a) a
#define CAT(a, b) a##b
#define CAT2(a, b) CAT(a, b)

#define OYL_MODULE_DECLARE(_module_name_, _module_class_) \
	extern "C" \
	__OYL_EXPORT_ATTR \
	::Oyl::IModuleInterface* \
	OYL_MODULE_INIT_FN_NAME(_module_name_)() \
	{ \
		return new _module_class_(); \
	} \
	extern "C" void DECLARE_MODULE_##_module_name_() { static_assert(std::string_view(#_module_name_) == OYL_CURRENT_MODULE); } \
	\
	extern "C" \
	__OYL_EXPORT_ATTR \
	void \
	OYL_MODULE_DEPS_FN_NAME(_module_name_)(int* a_depc, const char*** a_depv) \
	{ \
		static const char* deps[] = CAT2(EXPAND(OYL_CURRENT_MODULE_AS_MACRO), _DEPENDENCIES); \
		*a_depc = std::size(deps); \
		*a_depv = deps; \
	}
