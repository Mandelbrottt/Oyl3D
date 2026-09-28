#include "ModuleManager.h"

#include <unordered_map>

namespace Oyl
{
	struct ModuleInfo
	{
		String name;

		Platform::SharedLibraryHandle libHandle;
		IModuleInterface* interface;
	};

	struct ModuleManager::Impl
	{
		std::unordered_map<std::string, ModuleInfo> modules;
	};

	ModuleManager::ModuleManager()
		: m_impl(new Impl) {}

	ModuleManager::~ModuleManager()
	{
		delete m_impl;
	}

	ModuleManager&
	ModuleManager::Get()
	{
		static ModuleManager manager;
		return manager;
	}

	bool
	ModuleManager::LoadModule(const String& a_moduleName)
	{
		if (auto iter = m_impl->modules.find(a_moduleName); iter != m_impl->modules.end())
		{
			return true;
		}

		Platform::SharedLibraryHandle libHandle;
		Platform::LoadSharedLibrary(&libHandle, a_moduleName);
		if (libHandle == nullptr)
			return false;

		using NewModuleInterfaceFn = IModuleInterface*(*)();
		String symbolName = a_moduleName + String("_NewModuleInterface");
		auto getModuleInterface = (NewModuleInterfaceFn) Platform::GetSymbolFromSharedLibrary(libHandle, symbolName);

		ModuleInfo info;
		info.libHandle = libHandle;
		info.name = a_moduleName;
		info.interface = getModuleInterface();
		m_impl->modules.emplace(a_moduleName, std::move(info));

		return true;
	}

	IModuleInterface*
	ModuleManager::GetModuleInterface(const String& a_moduleName) const
	{
		if (auto iter = m_impl->modules.find(a_moduleName); iter != m_impl->modules.end())
		{
			return iter->second.interface;
		}
		return nullptr;
	}

	bool
	ModuleManager::UnloadModule(const String& a_moduleName)
	{
		if (auto iter = m_impl->modules.find(a_moduleName); iter != m_impl->modules.end())
		{
			using DeleteModuleInterfaceFn = void(*)(IModuleInterface*);

			String symbolName = a_moduleName + String("_DeleteModuleInterface");
			auto deleteModuleInterface =
				(DeleteModuleInterfaceFn) Platform::GetSymbolFromSharedLibrary(iter->second.libHandle, symbolName);
			deleteModuleInterface(iter->second.interface);

			m_impl->modules.erase(iter);
			return true;
		}
		return false;
	}

	bool
	ModuleManager::UnloadModuleUnsafe(const String& a_moduleName)
	{
		(void) a_moduleName;
		return false;
		//return TODO_IMPLEMENT_ME;
	}
}
