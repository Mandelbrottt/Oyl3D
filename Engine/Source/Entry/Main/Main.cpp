#include <Windows.h>
#include <iostream>

#include "Editor/main.h"

#include "Engine/main.h"

static void SetupConsole();

static void ShutdownConsole();

int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    pCmdLine,
	int       nCmdShow
)
{
	(void) hInstance;
	(void) hPrevInstance;
	(void) pCmdLine;
	(void) nCmdShow;

	SetupConsole();
	
	std::cout << "Hello World!\n";

	Something();

	SomethingElse();

	getchar();

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
	freopen_s(&inFileStream,  "CONIN$", "r", stdin);
	freopen_s(&outFileStream, "CONOUT$", "w", stdout);
	freopen_s(&errFileStream, "CONOUT$", "w", stderr);
}

void ShutdownConsole()
{	
	FreeConsole();
}
