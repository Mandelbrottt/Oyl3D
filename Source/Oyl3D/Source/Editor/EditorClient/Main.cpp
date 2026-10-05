#include <iostream>

#include <Core/CommandLine/CommandLine.h>
#include <Core/Logging/Logging.h>
#include <Core/Module/ModuleManager.h>

#ifndef OYL_EXPOSE_PROFILER_CONTROLS
#	define OYL_EXPOSE_PROFILER_CONTROLS
#endif
#include <Core/Profiling/Profiling.h>

int
main(int a_argc, char** a_argv)
{
#if defined(OYL_PROFILE)
	{
		// Allow users to dynamically set whether they want to profile over the network or not
		// By default, only work over localhost
		bool doProfileNetwork = false;
		for (int i = 0; i < a_argc; i++)
		{
			if (strcmp(a_argv[i], "profile_network"))
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

	OYL_PROFILER_INIT();
	OYL_PROFILE_FUNCTION();

	Oyl::ModuleManager::LoadModulePointer("Core");
	Oyl::ModuleManager::Get().LoadModule("SpdLogLogger");

	Oyl::CommandLine::Init(a_argc, a_argv);

	Oyl::Log("Testing {} {} {}", 1, 2, 3);

	Oyl::Internal::GetCurrentLogger()->Flush();

	Oyl::CommandLine::Shutdown();

	Oyl::ModuleManager::Get().UnloadModule("SpdLogLogger");
	Oyl::ModuleManager::Get().UnloadModule("Core");

	OYL_PROFILER_SHUTDOWN();
}

#if defined(_WIN32)
#include <Windows.h>

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

	int result = main(__argc, __argv);

	std::cin.get();

	fclose(inFileStream);
	fclose(outFileStream);
	fclose(errFileStream);

	FreeConsole();

	return result;
}

#endif
