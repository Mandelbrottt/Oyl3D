#include <Windows.h>
#include <cstdlib>
#include <filesystem>

#include <Core/Application/CommandLine.h>
#include <Core/Application/Main.h>
#include <Core/Logging/Logging.h>
#include <Core/Profiling/Profiler.h>

#include "Include.generated.h"

static void SetupConsole();

static void ShutdownConsole();

static bool g_running = true;

// ReSharper disable CppInconsistentNaming
int WINAPI WinMain(
	_In_ HINSTANCE     hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR         lpCmdLine,
	_In_ int           nShowCmd
) // ReSharper restore CppInconsistentNaming
{
	OYL_UNUSED(hInstance);
	OYL_UNUSED(hPrevInstance);
	OYL_UNUSED(lpCmdLine);
	OYL_UNUSED(nShowCmd);
	
	{
		std::vector<const char*> args;
		args.reserve(__argc - 1); // Skip exe name for commandline params
		for (int i = 1; i < __argc; i++)
		{
			args.push_back(__argv[i]);
		}

		Oyl::CommandLine::Detail::ParseCommandLine(args.size(), args.data());
	}
	
	SetupConsole();

#ifdef OYL_PROFILE
	// Allow users to dynamically set whether they want to profile over the network or not
	// By default, only work over localhost
	if (!Oyl::CommandLine::IsPresent("profile_network"))
	{
		SetEnvironmentVariableA("TRACY_ONLY_LOCALHOST", "1");
	}
#endif

	OYL_PROFILER_INIT();
	
	Oyl::Detail::CoreInitParameters initParams;

	initParams.onApplicationShouldQuitCallback = [] { g_running = false; };

	Oyl::Detail::SetShouldGameUpdate(
	#ifdef OYL_EDITOR
		false
	#else
		true
	#endif
	);

	const char* startupString = "Startup";
	OYL_FRAME_MARK_START(startupString);
	Oyl::Detail::Init(initParams);
	OYL_FRAME_MARK_END(startupString);
	
	while (g_running)
	{
		Oyl::Detail::Update();
		OYL_FRAME_MARK();
	}

	const char* shutdownString = "Shutdown";
	OYL_FRAME_MARK_START(shutdownString);
	Oyl::Detail::Shutdown();
	OYL_FRAME_MARK_END(shutdownString);

	ShutdownConsole();

	OYL_PROFILER_SHUTDOWN();

	return 0;
}

void SetupConsole()
{
	// Attach to the console if launched from the command line, otherwise create one
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();

	FILE* inFileStream;
	FILE* outFileStream;
	FILE* errFileStream;

	// Direct input and output to the console
	freopen_s(&inFileStream, "CONIN$", "r", stdin);
	freopen_s(&outFileStream, "CONOUT$", "w", stdout);
	freopen_s(&errFileStream, "CONOUT$", "w", stderr);
}

void ShutdownConsole()
{
	FreeConsole();
}
