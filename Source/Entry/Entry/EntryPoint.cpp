#include <Windows.h>
#include <iostream>

import Core;

static void SetupConsole();

static void ShutdownConsole();

// For now you need to manually specify additional
// https://www.reddit.com/r/VisualStudio/comments/qom7bg/has_anyone_gotten_c20_modules_working_across_vs/

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

	Rearm::Init();

	Rearm::Something();

	Rearm::SomethingElse();

	//do 
	//{
	//	extern void LoadFromDll();
	//	LoadFromDll();

	//	printf("press q to stop\n");
	//} while (getchar() != 'q');

	std::cin.get();
	
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
