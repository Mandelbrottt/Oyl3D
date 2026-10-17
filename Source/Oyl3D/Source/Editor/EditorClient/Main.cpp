#include <iostream>

#include <Application/Application.h>

#include <Core/CommandLine/CommandLine.h>
#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Time/Time.h>

#ifndef OYL_EXPOSE_PROFILER_CONTROLS
#	define OYL_EXPOSE_PROFILER_CONTROLS
#endif
#include <filesystem>

#include <Core/Profiling/Profiling.h>
#include <Core/Time/IFrameTimer.h>

#include "EditorClientApplication.h"

static Oyl::IFrameTimer* g_frameTimer = nullptr;

int
main(int a_argc, char** a_argv)
{
	OYL_PROFILER_INIT();
	OYL_PROFILE_FUNCTION();

	Oyl::Time::Internal::Init(*g_frameTimer);
	Oyl::CommandLine::Init(a_argc, a_argv);

	Oyl::EditorClientApplication application;
	application.RegisterOnUpdateCallback(Oyl::Time::Internal::Update);
	application.RegisterOnUpdateCallback([] { Oyl::Internal::GetCurrentLogger()->Flush(); });

	// Initial scan for modules
	auto cwd = std::filesystem::current_path().string();
	Oyl::ModuleManager::Get().AddEngineModuleDirectory(Oyl::StringView(cwd));
	Oyl::ModuleManager::Get().ScanModuleDirectories();
	application.RegisterOnUpdateCallback([] { Oyl::ModuleManager::Get().ProcessPendingAutoLoadRequests(); });

	// Register EditorClientApplication after initial module scan
	auto* applicationModule = Oyl::ModuleManager::LoadModulePointer<Oyl::IApplicationModule>("Application");
	applicationModule->RegisterApplication(&application);

	// Process pending auto-load requests
	Oyl::ModuleManager::Get().ProcessPendingAutoLoadRequests();

	int exitCode = applicationModule->RunApplication();

	Oyl::CommandLine::Shutdown();

	OYL_PROFILER_SHUTDOWN();

	return exitCode;
}

#if defined(OYL_WINDOWS)
#include <Windows.h>

#include <Core/Time/Windows/WindowsFrameTimer.h>

int
WINAPI
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();

	FILE* inFileStream;
	FILE* outFileStream;
	FILE* errFileStream;

	// Direct input and output to the console
	freopen_s(&inFileStream, "CONIN$", "r", stdin);
	freopen_s(&outFileStream, "CONOUT$", "w", stdout);
	freopen_s(&errFileStream, "CONOUT$", "w", stderr);

#if defined(OYL_PROFILE)
	{
		// Allow users to dynamically set whether they want to profile over the network or not
		// By default, only work over localhost
		bool doProfileNetwork = false;
		for (int i = 0; i < __argc; i++)
		{
			if (strcmp(__argv[i], "profile_network"))
			{
				doProfileNetwork = true;
				break;
			}
		}
		if (doProfileNetwork)
		{
		#if defined(OYL_WINDOWS)
			SetEnvironmentVariableA("TRACY_ONLY_LOCALHOST", "1");
		#endif
		}
	}
#endif

	g_frameTimer = new Oyl::WindowsFrameTimer;
	int result = main(__argc, __argv);

	fclose(inFileStream);
	fclose(outFileStream);
	fclose(errFileStream);

	FreeConsole();

	return result;
}

#endif
