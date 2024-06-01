#include <Windows.h>
#include <iostream>
#include <stdbool.h>

#include <Core/Application/Main.h>

static void SetupConsole();

static void ShutdownConsole();

static bool g_running = true;

// ReSharper disable CppInconsistentNaming
int WINAPI wWinMain(
	_In_ HINSTANCE     hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR        lpCmdLine,
	_In_ int           nShowCmd
) // ReSharper restore CppInconsistentNaming
{
	(void) hInstance;
	(void) hPrevInstance;
	(void) lpCmdLine;
	(void) nShowCmd;

	SetupConsole();
	
	Rearm::CoreInitParameters initParams;
	initParams.onApplicationShouldQuitCallback = [] { g_running = false; };

	Rearm::SetShouldGameUpdate(
	#ifdef R_EDITOR
		false
	#else
		true
	#endif
	);

	Rearm::Init(initParams);

	while (g_running)
	{
		Rearm::Update();
	}
	
	Rearm::Shutdown();

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
	freopen_s(&inFileStream,  "CONIN$",  "r", stdin);
	freopen_s(&outFileStream, "CONOUT$", "w", stdout);
	freopen_s(&errFileStream, "CONOUT$", "w", stderr);
}

void ShutdownConsole()
{	
	FreeConsole();
}
