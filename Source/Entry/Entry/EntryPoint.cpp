#include <Windows.h>
#include <filesystem>

#include <Core/Logging/Logging.h>
#include <Core/Application/CommandLine.h>
#include <Core/Application/Main.h>
#include <Core/Profiling/Profiler.h>

static void SetupConsole();

static void ShutdownConsole();

static bool                               g_running = true;
static std::vector<std::function<void()>> g_preInitLogCalls;

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

	std::vector<const char*> args;
	args.reserve(__argc - 1); // Skip exe name for commandline params
	for (int i = 1; i < __argc; i++)
	{
		args.push_back(__argv[i]);
	}

	Oyl::CommandLine::Detail::ParseCommandLine(args.size(), args.data());

	SetupConsole();

	Oyl::CoreInitParameters initParams;

	initParams.onApplicationShouldQuitCallback = [] { g_running = false; };

	Oyl::SetShouldGameUpdate(
	#ifdef OYL_EDITOR
		false
	#else
		true
	#endif
	);

	Oyl::Profiling::RegisterThreadName("Main");

	OYL_PROFILE_BEGIN_SESSION("Startup", "Debug/Profiling/OylProfile_Startup.json");
	Oyl::Init(initParams);
	OYL_PROFILE_END_SESSION();

	OYL_PROFILE_BEGIN_SESSION("Running", "Debug/Profiling/OylProfile_Runtime.json");
	while (g_running)
	{
		Oyl::Update();
	}
	OYL_PROFILE_END_SESSION();

	OYL_PROFILE_BEGIN_SESSION("Shutdown", "Debug/Profiling/OylProfile_Shutdown.json");
	Oyl::Shutdown();
	OYL_PROFILE_END_SESSION();

	ShutdownConsole();

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
