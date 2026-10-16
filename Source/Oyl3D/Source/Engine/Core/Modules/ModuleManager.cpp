#include "ModuleManager.h"

#include <filesystem>
#include <unordered_map>

#include "Core/CommandLine/CommandLine.h"
#include "Core/Containers/Array.h"
#include "Core/Logging/Logging.h"
#include "Core/Profiling/Profiling.h"

namespace Oyl
{
	struct ModuleInfo
	{
		String name;
		String libPath;

		Array<String> dependencies;
		IModuleInterface* interfacePointer;

		Platform::SharedLibraryHandle* sharedLibHandle;
		int refCount = 0;
	};

	struct ModuleManager::Impl
	{
		std::unordered_map<std::string, ModuleInfo> modules;
		Array<String> modulesToAutoload;

		Array<String> engineModuleDirectories;
		Array<String> userModuleDirectories;

		bool moduleDirectoriesAreDirty = true;

		ModuleInfo*
		FindModule(const String& a_moduleName);

		void
		DiscoverModule(const String& a_libPath);
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
		OYL_PROFILE_FUNCTION();

		String moduleName = a_moduleName;

		auto modulePath = std::filesystem::path((const std::string&) a_moduleName);
		if (modulePath != modulePath.stem())
			moduleName = modulePath.stem().string();

		auto iter = m_impl->modules.find(moduleName);
		if (iter == m_impl->modules.end())
		{
			LogError("Trying to load module \"{}\" that hasn't been discovered!");
			if (a_outResult)
				*a_outResult = ModuleLoadResult::Failure_LibraryNotFound;
			return nullptr;
		}
		auto& moduleInfo = iter->second;
		if (moduleInfo.refCount == 0)
		{ // Module is not loaded
			// Load dependencies
			for (const auto& dependencyName : moduleInfo.dependencies)
			{
				auto dependencyIter = m_impl->modules.find(dependencyName);
				if (dependencyIter == m_impl->modules.end())
				{
					LogError("Dependency \"{}\" in module \"{}\" not found!");
				} else
				{
					LoadModule(dependencyIter->second.name);
				}
			}

			// Start the module
			moduleInfo.interfacePointer->OnStartModule();
			LogDebug("Loaded Module \"{}\"", moduleName);
		}
		moduleInfo.refCount++;

		if (a_outResult)
			*a_outResult = ModuleLoadResult::Success;

		return moduleInfo.interfacePointer;
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
		OYL_PROFILE_FUNCTION();

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

			LogDebug("Unloaded Module \"{}\"", moduleInfo.name);

			// Unload dependencies after requested module so that we unload in reverse order
			for (const auto& dependency : moduleInfo.dependencies)
				UnloadModule(dependency);

			return withResult(true, ModuleUnloadResult::Success);
		}

		return withResult(false, ModuleUnloadResult::Failure_StillInUse);
	}

	void
	ModuleManager::AddEngineModuleDirectory(StringView a_directory)
	{
		auto absPath = std::filesystem::absolute(std::string(a_directory)).string();
		auto iter = std::ranges::find(m_impl->engineModuleDirectories, absPath);
		if (iter == m_impl->engineModuleDirectories.end())
		{
			LogTrace("Adding \"{}\" as Engine Module Directory", a_directory);
			m_impl->engineModuleDirectories.Add(absPath);
			m_impl->moduleDirectoriesAreDirty = true;
		}
	}

	void
	ModuleManager::RemoveEngineModuleDirectory(StringView a_directory)
	{
		auto absPath = std::filesystem::absolute(std::string(a_directory)).string();
		auto iter = std::ranges::find(m_impl->engineModuleDirectories, absPath);
		if (iter != m_impl->engineModuleDirectories.end())
		{
			LogTrace("Removing \"{}\" as Engine Module Directory", a_directory);
			m_impl->engineModuleDirectories.erase(iter);
			m_impl->moduleDirectoriesAreDirty = true;
		}
	}

	void
	ModuleManager::AddUserModuleDirectory(StringView a_directory)
	{
		auto absPath = std::filesystem::absolute(std::string(a_directory)).string();
		auto iter = std::ranges::find(m_impl->userModuleDirectories, absPath);
		if (iter == m_impl->userModuleDirectories.end())
		{
			LogTrace("Adding \"{}\" as User Module Directory", a_directory);
			m_impl->userModuleDirectories.Add(absPath);
			m_impl->moduleDirectoriesAreDirty = true;
		}
	}

	void
	ModuleManager::RemoveUserModuleDirectory(StringView a_directory)
	{
		auto absPath = std::filesystem::absolute((std::string&) a_directory).string();
		auto iter = std::ranges::find(m_impl->userModuleDirectories, absPath);
		if (iter != m_impl->userModuleDirectories.end())
		{
			LogTrace("Removing \"{}\" as User Module Directory", a_directory);
			m_impl->userModuleDirectories.erase(iter);
			m_impl->moduleDirectoriesAreDirty = true;
		}
	}

	void
	ModuleManager::ScanModuleDirectories()
	{
		OYL_PROFILE_FUNCTION();
		LogTrace("Scanning Engine and Module Directories");
		auto scanAndLoadFromDirsInList = [&](const Array<String>& a_list)
		{
			// Cache list in case loading a module adds to the list
			auto list = a_list;
			for (const auto& directory : a_list)
			{
				auto iter = std::filesystem::directory_iterator((const std::string&) directory);
				for (const auto& dir_entry : iter)
				{
					if (!dir_entry.exists())
						continue;
					if (!dir_entry.is_regular_file())
						continue;
					auto extension = dir_entry.path().extension();
					if (extension == ".dll" || extension == ".dylib" || extension == ".so")
					{
						m_impl->DiscoverModule(dir_entry.path().string());
					}
				}
			}
		};
		scanAndLoadFromDirsInList(m_impl->engineModuleDirectories);
		scanAndLoadFromDirsInList(m_impl->userModuleDirectories);

		m_impl->moduleDirectoriesAreDirty = false;
	}

	void
	ModuleManager::ProcessPendingAutoLoadRequests()
	{
		for (const auto& moduleName : m_impl->modulesToAutoload)
		{
			LoadModule(moduleName);
		}

		m_impl->modulesToAutoload.clear();
	}

	ModuleInfo*
	ModuleManager::Impl::FindModule(const String& a_moduleName)
	{
		auto iter = modules.find(a_moduleName);
		if (iter == modules.end())
			return nullptr;

		return &iter->second;
	}

	void
	ModuleManager::Impl::DiscoverModule(const String& a_libPath)
	{
		String moduleName = std::filesystem::path((const std::string&) a_libPath).stem().string();

		auto iter = modules.find(moduleName);
		if (iter != modules.end())
		{
			// Module has already been discovered, error if it refers to a different shared library
			if (iter->second.libPath != a_libPath)
				LogError("Duplicate module \"{}\" at \"{}\" has already been discovered at \"{}\"!",
				         moduleName,
				         a_libPath,
				         iter->second.libPath);

			return;
		}

		ModuleInfo moduleInfo;
		moduleInfo.name = moduleName;
		moduleInfo.libPath = a_libPath;

		// Get handle to shared library
		moduleInfo.sharedLibHandle = Platform::LoadSharedLibrary(a_libPath);
		assert(moduleInfo.sharedLibHandle);

		// Get handle to creation function
		using ModuleInitFn = IModuleInterface*(*)();
		String initSymbolName = String("ModuleInit_") + moduleName;
		auto moduleInitFn =
			(ModuleInitFn) Platform::GetSymbolFromSharedLibrary(moduleInfo.sharedLibHandle, initSymbolName);
		if (moduleInitFn == nullptr)
		{
			// Not an OylModule, unload and continue;
			FreeSharedLibrary(moduleInfo.sharedLibHandle);
			return;
		}

		// Get handle to module interface
		IModuleInterface* interface = moduleInitFn();
		assert(interface && "If ModuleInit_ is found, valid interface should be returned");
		moduleInfo.interfacePointer = interface;

		// Get handle to dependencies function
		using ModuleDepsFn = void(*)(int*, const char***);
		String depsSymbolName = String("ModuleDeps_") + moduleName;
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
					moduleInfo.dependencies.emplace_back(depStr);
				}
			}
		}

		// Module discovery successful
		modules.try_emplace(moduleName, std::move(moduleInfo));
		LogTrace("Discovered Module \"{}\" at {}", moduleName, a_libPath);

		// Should we autoload this module?
		if (interface->ShouldAutoload())
			modulesToAutoload.Add(moduleName);
	}
}
