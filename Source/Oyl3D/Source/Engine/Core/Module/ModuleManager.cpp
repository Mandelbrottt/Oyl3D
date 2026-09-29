#include "ModuleManager.h"

#include <unordered_map>

namespace Oyl
{
	struct ModuleInfo
	{
		String moduleName;

		Platform::SharedLibraryHandle sharedLibHandle;
		IModuleInterface* modulePointer;
	};

	struct ModuleManager::Impl
	{
		std::unordered_map<std::string, ModuleInfo> modules;

		ModuleInfo*
		FindModule(const String& a_moduleName);
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

	IModuleInterface*
	ModuleManager::LoadModule(const String& a_moduleName)
	{
		if (auto iter = m_impl->modules.find(a_moduleName); iter != m_impl->modules.end())
		{
			return iter->second.modulePointer;
		}

		// Get handle to shared library
		Platform::SharedLibraryHandle libHandle;
		Platform::LoadSharedLibrary(&libHandle, a_moduleName);
		if (libHandle == nullptr)
			return nullptr;

		// Get handle to creation function
		using ModuleDepsFn = void(*)(int*, const char***);
		String depsSymbolName = String("ModuleDeps_") + a_moduleName;
		auto moduleDepsFn = (ModuleDepsFn) Platform::GetSymbolFromSharedLibrary(libHandle, depsSymbolName);
		if (moduleDepsFn == nullptr)
			return nullptr;

		std::vector<std::string> deps;
		int nDeps;
		const char** depsPtr;
		moduleDepsFn(&nDeps, &depsPtr);
		deps.reserve(nDeps);
		for (int i = 0; i < nDeps; i++)
			deps.emplace_back(depsPtr[i]);
		for (const auto& dep : deps)
			puts(dep.c_str());

		// Get handle to creation function
		using ModuleInitFn = IModuleInterface*(*)();
		String initSymbolName = String("ModuleInit_") + a_moduleName;
		auto moduleInitFn = (ModuleInitFn) Platform::GetSymbolFromSharedLibrary(libHandle, initSymbolName);
		if (moduleInitFn == nullptr)
			return nullptr;

		// Get handle to module interface
		IModuleInterface* modulePtr = moduleInitFn();
		if (modulePtr == nullptr)
			return nullptr;

		// Module load succeeded!
		ModuleInfo& moduleInfo = m_impl->modules.emplace(a_moduleName, ModuleInfo {}).first->second;
		moduleInfo.sharedLibHandle = libHandle;
		moduleInfo.moduleName = a_moduleName;
		moduleInfo.modulePointer = modulePtr;

		moduleInfo.modulePointer->OnStartModule();

		return modulePtr;
	}

	IModuleInterface*
	ModuleManager::GetModule(const String& a_moduleName) const
	{
		if (auto iter = m_impl->modules.find(a_moduleName); iter != m_impl->modules.end())
		{
			return iter->second.modulePointer;
		}
		return nullptr;
	}

	bool
	ModuleManager::UnloadModule(const String& a_moduleName)
	{
		if (auto iter = m_impl->modules.find(a_moduleName); iter != m_impl->modules.end())
		{
			ModuleInfo& moduleInfo = iter->second;

			moduleInfo.modulePointer->OnStopModule();
			delete moduleInfo.modulePointer;
			moduleInfo.modulePointer = nullptr;

			Platform::FreeSharedLibrary(moduleInfo.sharedLibHandle);
			m_impl->modules.erase(iter);
			return true;
		}

		return false;
	}

	ModuleInfo*
	ModuleManager::Impl::FindModule(const String& a_moduleName)
	{
		auto iter = modules.find(a_moduleName);
		if (iter == modules.end())
			return nullptr;

		return &iter->second;
	}
}
