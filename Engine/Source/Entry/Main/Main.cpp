#include <Windows.h>
#include <iostream>

#if REFLY_EDITOR
	#include "Editor/main.h"
#endif

#include "Engine/main.h"

static void SetupConsole();

static void ShutdownConsole();

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    pCmdLine,
	_In_ int       nCmdShow
)
{
	(void) hInstance;
	(void) hPrevInstance;
	(void) pCmdLine;
	(void) nCmdShow;

	SetupConsole();
	
	std::cout << "Hello World!\n";

#if REFLY_EDITOR
	Something();
#endif
	
	SomethingElse();

	std::cin.get();

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
