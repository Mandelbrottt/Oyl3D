#pragma once

#include "ModuleInterface.h"

#include "Core/Platform/SharedLibrary.h"
#include "Core/Preprocessor/Common.h"

namespace Oyl
{
	using ModuleHandle = struct _ModuleHandle*;

	enum class ModuleLoadResult : uint32_t
	{
		Success = 0,

		Failure_LibraryNotFound,
		Failure_InitFunctionNotFound,
		Failure_InterfaceWasNull,

		Failure_DependencyFailure,
		Failure_DependencyNotFound,
	};

	enum class ModuleUnloadResult : uint32_t
	{
		Success = 0,

		Failure_NotFound,
		Failure_StillInUse,
	};

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
		LoadModule(const String& a_moduleName, ModuleLoadResult* a_outResult = nullptr);

		CORE_EXPORT
		IModuleInterface*
		GetModule(const String& a_moduleName) const;

		CORE_EXPORT
		bool
		UnloadModule(const String& a_moduleName, ModuleUnloadResult* a_outResult = nullptr);

		template<typename TInterface = IModuleInterface>
			requires (std::is_convertible_v<TInterface*, IModuleInterface*>)
		static
		TInterface*
		LoadModulePointer(const String& a_moduleName, ModuleLoadResult* a_outResult = nullptr)
		{
			return static_cast<TInterface*>(Get().LoadModule(a_moduleName, a_outResult));
		}

		template<typename TInterface = IModuleInterface>
			requires (std::is_convertible_v<TInterface*, IModuleInterface*>)
		static
		TInterface*
		GetModulePointer(const String& a_moduleName)
		{
			return static_cast<TInterface*>(Get().GetModule(a_moduleName));
		}

		CORE_EXPORT
		void
		AddEngineModuleDirectory(StringView a_directory);

		CORE_EXPORT
		void
		RemoveEngineModuleDirectory(StringView a_directory);

		CORE_EXPORT
		void
		AddUserModuleDirectory(StringView a_directory);

		CORE_EXPORT
		void
		RemoveUserModuleDirectory(StringView a_directory);

		CORE_EXPORT
		void
		ScanAndLoadFromModuleDirectories();

	private:
		struct Impl;
		Impl* m_impl;
	};
}

#define OYL_MODULE_INIT_FN_NAME(_module_name_) ModuleInit_##_module_name_
#define OYL_MODULE_DEPS_FN_NAME(_module_name_) ModuleDeps_##_module_name_
#define OYL_MODULE_DECLARE_FN_NAME(_module_name_) _DECLARE_MODULE_##_module_name_
#define OYL_MODULE_DEPS_MACRO(_module_name_) _module_name_

#define EXPAND(a) a
#define CAT(a, b) a##b
#define CAT2(a, b) CAT(a, b)

#define OYL_MODULE_DECLARE(_module_name_, _module_class_) \
	extern "C" \
	__OYL__EXPORT_ATTR \
	::Oyl::IModuleInterface* \
	OYL_MODULE_INIT_FN_NAME(_module_name_)() \
	{ \
		return new _module_class_(); \
	} \
	extern "C" \
	void \
	OYL_MODULE_DECLARE_FN_NAME(_module_name_)() { static_assert(::Oyl::StringView(#_module_name_) == OYL_MODULE_CURRENT); } \
	\
	extern "C" \
	__OYL__EXPORT_ATTR \
	void \
	OYL_MODULE_DEPS_FN_NAME(_module_name_)(int* a_depc, const char*** a_depv) \
	{ \
		static const char* deps[] = CAT2(EXPAND(OYL_MODULE_CURRENT_AS_MACRO), _DEPENDENCIES); \
		if (**deps == '\0') \
		{ \
			*a_depc = 0; \
			*a_depv = nullptr; \
		} else { \
			*a_depc = ::std::size(deps); \
			*a_depv = deps; \
		} \
	}
