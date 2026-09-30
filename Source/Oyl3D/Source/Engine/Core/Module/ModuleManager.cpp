#include "ModuleManager.h"

#include <unordered_map>

#include "Core/Containers/Array.h"

namespace Oyl
{
	struct ModuleInfo
	{
		String name;
		Array<String> dependencies;
		IModuleInterface* interfacePointer;

		Platform::SharedLibraryHandle sharedLibHandle;
		int refCount = 0;
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
	ModuleManager::LoadModule(const String& a_moduleName, ModuleLoadResult* a_outResult)
	{
		if (auto iter = m_impl->modules.find(a_moduleName); iter != m_impl->modules.end())
		{
			ModuleInfo& info = iter->second;
			info.refCount++;
			*a_outResult = ModuleLoadResult::Success;
			return info.interfacePointer;
		}

		ModuleInfo moduleInfo;
		moduleInfo.name = a_moduleName;
		moduleInfo.refCount = 1;

		// Cleanup code if the module fails to load
		auto failWithResult = [&](IModuleInterface* a_interface, ModuleLoadResult a_result)
		{
			if (moduleInfo.sharedLibHandle)
			{
				Platform::FreeSharedLibrary(moduleInfo.sharedLibHandle);
				moduleInfo.sharedLibHandle = nullptr;
			}
			if (a_outResult)
				*a_outResult = a_result;
			return a_interface;
		};

		// Get handle to shared library
		Platform::LoadSharedLibrary(&moduleInfo.sharedLibHandle, a_moduleName);
		if (moduleInfo.sharedLibHandle == nullptr)
			return failWithResult(nullptr, ModuleLoadResult::Failure_LibraryNotFound);

		// Get handle to dependencies function
		using ModuleDepsFn = void(*)(int*, const char***);
		String depsSymbolName = String("ModuleDeps_") + a_moduleName;
		auto moduleDepsFn =
			(ModuleDepsFn) Platform::GetSymbolFromSharedLibrary(moduleInfo.sharedLibHandle, depsSymbolName);

		// If ModuleDeps_ function exists, get list of dependencies
		if (moduleDepsFn != nullptr)
		{
			int nDeps = 0;
			const char** depsPtr = nullptr;
			moduleDepsFn(&nDeps, &depsPtr);
			if (depsPtr && nDeps > 0)
			{
				moduleInfo.dependencies.reserve(nDeps);
				for (int i = 0; i < nDeps; i++)
				{
					const char* depStr = depsPtr[i];

					// Try to load the dependency
					ModuleLoadResult dependencyLoadResult;
					auto dependencyInterface = LoadModule(depStr, &dependencyLoadResult);
					if (!dependencyInterface)
						switch (dependencyLoadResult)
						{
							case ModuleLoadResult::Failure_LibraryNotFound:
								return failWithResult(nullptr, ModuleLoadResult::Failure_DependencyNotFound);
							case ModuleLoadResult::Failure_InitFunctionNotFound:
							case ModuleLoadResult::Failure_InterfaceWasNull:
								return failWithResult(nullptr, ModuleLoadResult::Failure_DependencyFailure);
							default:
								throw "ModuleLoadResult case not implemented in Dependency Resolution!";
						}

					moduleInfo.dependencies.emplace_back(depStr);
				}
			}
		}

		// Get handle to creation function
		using ModuleInitFn = IModuleInterface*(*)();
		String initSymbolName = String("ModuleInit_") + a_moduleName;
		auto moduleInitFn =
			(ModuleInitFn) Platform::GetSymbolFromSharedLibrary(moduleInfo.sharedLibHandle, initSymbolName);
		if (moduleInitFn == nullptr)
			return failWithResult(nullptr, ModuleLoadResult::Failure_InitFunctionNotFound);

		// Get handle to module interface
		IModuleInterface* interface = moduleInitFn();
		if (interface == nullptr)
			return failWithResult(nullptr, ModuleLoadResult::Failure_InterfaceWasNull);
		moduleInfo.interfacePointer = interface;

		// Module load succeeded!
		m_impl->modules.emplace(a_moduleName, std::move(moduleInfo));
		interface->OnStartModule();
		if (a_outResult)
			*a_outResult = ModuleLoadResult::Success;
		return interface;
	}

	IModuleInterface*
	ModuleManager::GetModule(const String& a_moduleName) const
	{
		if (auto iter = m_impl->modules.find(a_moduleName); iter != m_impl->modules.end())
		{
			return iter->second.interfacePointer;
		}
		return nullptr;
	}

	bool
	ModuleManager::UnloadModule(const String& a_moduleName, ModuleUnloadResult* a_outResult)
	{
		auto withResult = [&](bool a_didUnload, ModuleUnloadResult a_result)
		{
			if (a_outResult)
				*a_outResult = a_result;
			return a_didUnload;
		};

		auto iter = m_impl->modules.find(a_moduleName);
		if (iter == m_impl->modules.end())
			return withResult(false, ModuleUnloadResult::Failure_NotFound);

		ModuleInfo& moduleInfo = iter->second;
		moduleInfo.refCount--;
		if (moduleInfo.refCount == 0)
		{
			moduleInfo.interfacePointer->OnStopModule();
			delete moduleInfo.interfacePointer;
			moduleInfo.interfacePointer = nullptr;

			// Unload dependencies after requested module so that we unload in reverse order
			for (const auto& dependency : moduleInfo.dependencies)
				UnloadModule(dependency);

			Platform::FreeSharedLibrary(moduleInfo.sharedLibHandle);
			m_impl->modules.erase(iter);
			return withResult(true, ModuleUnloadResult::Success);
		}

		return withResult(false, ModuleUnloadResult::Failure_StillInUse);
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
